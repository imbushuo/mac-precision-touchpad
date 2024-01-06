// Input.c: Input handler routines

#include <Driver.h>
#include "Input.tmh"

VOID
PtpFilterInputProcessRequest(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status;
	PDEVICE_CONTEXT deviceContext;

	deviceContext = PtpFilterGetContext(Device);
	status = WdfRequestForwardToIoQueue(Request, deviceContext->HidReadQueue);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_INPUT, "%!FUNC! WdfRequestForwardToIoQueue fails, status = %!STATUS!", status);
		WdfRequestComplete(Request, status);
		return;
	}

	// Only issue request when fully configured.
	// Otherwise we will let power recovery process to triage it
	if (deviceContext->DeviceConfigured == TRUE) {
		PtpFilterInputIssueTransportRequest(Device);
	}
}

VOID
PtpFilterWorkItemCallback(
	_In_ WDFWORKITEM WorkItem
)
{
	WDFDEVICE Device = WdfWorkItemGetParentObject(WorkItem);
	PtpFilterInputIssueTransportRequest(Device);
}

VOID
PtpFilterInputIssueTransportRequest(
	_In_ WDFDEVICE Device
)
{
	NTSTATUS status;
	PDEVICE_CONTEXT deviceContext;

	WDF_OBJECT_ATTRIBUTES attributes;
	WDFREQUEST hidReadRequest;
	WDFMEMORY hidReadOutputMemory;
	PWORKER_REQUEST_CONTEXT requestContext;
	BOOLEAN requestStatus = FALSE;

	deviceContext = PtpFilterGetContext(Device);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WORKER_REQUEST_CONTEXT);
	attributes.ParentObject = Device;
	status = WdfRequestCreate(&attributes, deviceContext->HidIoTarget, &hidReadRequest);
	if (!NT_SUCCESS(status)) {
		// This can fail for Bluetooth devices. We will set up a 3 second timer for retry triage.
		// Typically this should not fail for USB transport.
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfRequestCreate fails, status = %!STATUS!", status);
		deviceContext->DeviceConfigured = FALSE;
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_SEC(3));
		return;
	}

	status = WdfMemoryCreateFromLookaside(deviceContext->HidReadBufferLookaside, &hidReadOutputMemory);
	if (!NT_SUCCESS(status)) {
		// tbh if you fail here, something seriously went wrong...request a restart.
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfMemoryCreateFromLookaside fails, status = %!STATUS!", status);
		WdfObjectDelete(hidReadRequest);
		WdfDeviceSetFailed(deviceContext->Device, WdfDeviceFailedAttemptRestart);
		return;
	}

	// Assign context information
	// And format HID read request.
	requestContext = WorkerRequestGetContext(hidReadRequest);
	requestContext->DeviceContext = deviceContext;
	requestContext->RequestMemory = hidReadOutputMemory;
	status = WdfIoTargetFormatRequestForInternalIoctl(deviceContext->HidIoTarget, hidReadRequest,
		IOCTL_HID_READ_REPORT, NULL, 0, hidReadOutputMemory, 0);
	if (!NT_SUCCESS(status)) {
		// tbh if you fail here, something seriously went wrong...request a restart.
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfIoTargetFormatRequestForInternalIoctl fails, status = %!STATUS!", status);

		if (hidReadOutputMemory != NULL) {
			WdfObjectDelete(hidReadOutputMemory);
		}

		if (hidReadRequest != NULL) {
			WdfObjectDelete(hidReadRequest);
		}

		WdfDeviceSetFailed(deviceContext->Device, WdfDeviceFailedAttemptRestart);
		return;
	}

	// Set callback
	WdfRequestSetCompletionRoutine(hidReadRequest, PtpFilterInputRequestCompletionCallback, requestContext);

	requestStatus = WdfRequestSend(hidReadRequest, deviceContext->HidIoTarget, NULL);
	if (!requestStatus) {
		// Retry after 3 seconds, in case this is a transportation issue.
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! PtpFilterInputIssueTransportRequest request failed to sent");
		deviceContext->DeviceConfigured = FALSE;
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_SEC(3));

		if (hidReadOutputMemory != NULL) {
			WdfObjectDelete(hidReadOutputMemory);
		}

		if (hidReadRequest != NULL) {
			WdfObjectDelete(hidReadRequest);
		}
	}
}

VOID
PtpFilterInputRequestCompletionCallback(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
)
{
	PWORKER_REQUEST_CONTEXT requestContext;
	PDEVICE_CONTEXT deviceContext;
	NTSTATUS status;

	WDFREQUEST ptpRequest;
	PTP_REPORT ptpOutputReport;
	WDFMEMORY  ptpRequestMemory;

	size_t responseLength;
	PUCHAR responseBuffer;

	UINT32 timestamp;
	size_t raw_n = 0;
	INT x, y = 0;

	const TRACKPAD_FINGER_TYPE5* f;
	const TRACKPAD_REPORT_TYPE5* mt_report;

	UNREFERENCED_PARAMETER(Target);
	
	requestContext = (PWORKER_REQUEST_CONTEXT)Context;
	deviceContext = requestContext->DeviceContext;
	responseLength = (size_t)(LONG)WdfRequestGetInformation(Request);
	responseBuffer = WdfMemoryGetBuffer(Params->Parameters.Ioctl.Output.Buffer, NULL);

	// Pre-flight check 0: Right now we only have Magic Trackpad 2 (BT and USB)
	if (deviceContext->VendorID != HID_VID_APPLE_USB && deviceContext->VendorID != HID_VID_APPLE_BT) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_INPUT, "%!FUNC! Unsupported device entered this routine");
		WdfDeviceSetFailed(deviceContext->Device, WdfDeviceFailedNoRestart);
		goto cleanup;
	}

	// Pre-flight check 1: if size is 0, this is not something we need. Ignore the read, and issue next request.
	if (responseLength <= 0) {
		WdfWorkItemEnqueue(requestContext->DeviceContext->HidTransportRecoveryWorkItem);
		goto cleanup;
	}

	// Pre-flight check 2: the response size should be sane
	if (responseLength < sizeof(TRACKPAD_REPORT_TYPE5) || (responseLength - sizeof(TRACKPAD_REPORT_TYPE5)) % sizeof(TRACKPAD_FINGER_TYPE5) != 0) {
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_INPUT, "%!FUNC! Malformed input received. Length = %llu. Attempt to reconfigure the device.", responseLength);
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_SEC(3));
		goto cleanup;
	}

	// Read report and fulfill PTP request. If no report is found, just exit.
	status = WdfIoQueueRetrieveNextRequest(deviceContext->HidReadQueue, &ptpRequest);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_INPUT, "%!FUNC! WdfIoQueueRetrieveNextRequest failed with %!STATUS!", status);
		goto cleanup;
	}

	mt_report = (TRACKPAD_REPORT_TYPE5*) responseBuffer;
	timestamp = (mt_report->TimestampHigh << 5) | mt_report->TimestampLow;

	// Report header
	ptpOutputReport.ReportID = REPORTID_MULTITOUCH;
	ptpOutputReport.IsButtonClicked = (UCHAR) mt_report->Button;
	ptpOutputReport.ScanTime = (USHORT) timestamp * 10;

	// Report required content
	// Touch
	raw_n = (responseLength - sizeof(TRACKPAD_REPORT_TYPE5)) / sizeof(TRACKPAD_FINGER_TYPE5);
	if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;
	ptpOutputReport.ContactCount = (UCHAR) raw_n;
	for (size_t i = 0; i < raw_n; i++) {
		f = &mt_report->Fingers[i];

		// Sign extend
		x = (SHORT)(f->AbsoluteX << 3) >> 3;
		y = -(SHORT)(f->AbsoluteY << 3) >> 3;
		x = (x - deviceContext->X.min) > 0 ? (x - deviceContext->X.min) : 0;
		y = (y - deviceContext->Y.min) > 0 ? (y - deviceContext->Y.min) : 0;

		ptpOutputReport.Contacts[i].ContactID = f->Id;
		ptpOutputReport.Contacts[i].X = (USHORT)x;
		ptpOutputReport.Contacts[i].Y = (USHORT)y;
		
		// 0x1 = Transition between states
		// 0x2 = Floating finger
		// 0x4 = Contact/Valid
		// I've gotten 0x6 if I press on the trackpad and then keep my finger close
		// Note: These values come from my MBP9,2. These also are valid on my MT2
		ptpOutputReport.Contacts[i].TipSwitch = (f->State & 0x4) && !(f->State & 0x2);

		// The Microsoft spec says reject any input larger than 25mm. This is not ideal
		// for Magic Trackpad 2 - so we raised the threshold a bit higher.
		// Or maybe I used the wrong unit? IDK
		CHAR valid_size = ((SHORT)(f->TouchMinor) << 1) < 345 && ((SHORT)(f->TouchMinor) << 1) < 345;

		// 1 = thumb, 2 = index, etc etc
		// 6 = palm on MT2, 7 = palm on my MBP9,2 (why are these different?)
		CHAR valid_finger = f->Finger != 6;
		ptpOutputReport.Contacts[i].Confidence = valid_size && valid_finger;
	}

	status = WdfRequestRetrieveOutputMemory(ptpRequest, &ptpRequestMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_INPUT, "%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		WdfDeviceSetFailed(deviceContext->Device, WdfDeviceFailedAttemptRestart);
		goto cleanup;
	}

	status = WdfMemoryCopyFromBuffer(ptpRequestMemory, 0, (PVOID) &ptpOutputReport, sizeof(PTP_REPORT));
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_INPUT, "%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		WdfDeviceSetFailed(deviceContext->Device, WdfDeviceFailedAttemptRestart);
		goto cleanup;
	}

	WdfRequestSetInformation(ptpRequest, sizeof(PTP_REPORT));
	WdfRequestComplete(ptpRequest, status);

cleanup:
	// Cleanup
	WdfObjectDelete(Request);
	if (requestContext->RequestMemory != NULL) {
		WdfObjectDelete(requestContext->RequestMemory);
	}

	// We don't issue new request here (unless it's a spurious request - which is handled earlier) to
	// keep the request pipe go through one-way.
}
