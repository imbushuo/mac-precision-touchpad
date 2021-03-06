// Diagnostics.c: Debug and diagnostics facilities

#include <Driver.h>
#include "Diagnostics.tmh"

VOID PtpFilterDiagnosticsInitializeContinuousRead(
	_In_ WDFDEVICE Device
)
{
	NTSTATUS status = STATUS_SUCCESS;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");
	
	// Issue a request to underlying HID target. Then start to have a continus stream reading it
	PtpFilterDiagnosticsInputIssueRequest(Device);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
}

VOID
PtpFilterDiagnosticsInputIssueRequest(
	_In_ WDFDEVICE Device
)
{
	NTSTATUS status;
	PDEVICE_CONTEXT deviceContext;
	WDF_OBJECT_ATTRIBUTES attributes;
	BOOLEAN requestStatus = FALSE;
	WDFREQUEST hidReadRequest;
	WDFMEMORY hidReadOutputMemory;
	PWORKER_REQUEST_CONTEXT requestContext;

	deviceContext = PtpFilterGetContext(Device);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, WORKER_REQUEST_CONTEXT);
	attributes.ParentObject = Device;

	status = WdfRequestCreate(&attributes, deviceContext->HidIoTarget, &hidReadRequest);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfRequestCreate fails, status = %!STATUS!",status);
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_US(200));
		return;
	}

	status = WdfMemoryCreateFromLookaside(deviceContext->HidReadBufferLookaside, &hidReadOutputMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfMemoryCreateFromLookaside fails, status = %!STATUS!", status);
		WdfObjectDelete(hidReadRequest);
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_US(200));
		return;
	}

	// Assign context information
	requestContext = WorkerRequestGetContext(hidReadRequest);
	requestContext->DeviceContext = deviceContext;
	requestContext->RequestMemory = hidReadOutputMemory;

	// Invoke HID read request to the device.
	status = WdfIoTargetFormatRequestForInternalIoctl(deviceContext->HidIoTarget, hidReadRequest,
		IOCTL_HID_READ_REPORT, NULL, 0, hidReadOutputMemory, 0);

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfIoTargetFormatRequestForInternalIoctl fails, status = %!STATUS!", status);

		if (hidReadOutputMemory != NULL) {
			WdfObjectDelete(hidReadOutputMemory);
		}

		if (hidReadRequest != NULL) {
			WdfObjectDelete(hidReadRequest);
		}

		return;
	}

	WdfRequestSetCompletionRoutine(
		hidReadRequest,
		PtpFilterDiagnosticsRequestCompletionRoutine,
		requestContext
	);

	requestStatus = WdfRequestSend(
		hidReadRequest,
		deviceContext->HidIoTarget,
		NULL
	);

	if (!requestStatus)
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! AmtPtpSpiInputRoutineWorker request failed to sent");
		WdfTimerStart(deviceContext->HidTransportRecoveryTimer, WDF_REL_TIMEOUT_IN_US(50));

		if (hidReadOutputMemory != NULL) {
			WdfObjectDelete(hidReadOutputMemory);
		}

		if (hidReadRequest != NULL) {
			WdfObjectDelete(hidReadRequest);
		}
	}
}

VOID
PtpFilterDiagnosticsRequestCompletionRoutine(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
)
{
	PWORKER_REQUEST_CONTEXT requestContext;
	PDEVICE_CONTEXT deviceContext;
	LONG responseLength;

	UNREFERENCED_PARAMETER(Request);
	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Params);

	requestContext = (PWORKER_REQUEST_CONTEXT)Context;
	deviceContext = requestContext->DeviceContext;

	// Decode request
	responseLength = (LONG) WdfRequestGetInformation(Request);
	if (responseLength > 0) {
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_INPUT, "Request received with size %d", responseLength);
	}

	// Cleanup
	WdfObjectDelete(Request);
	if (requestContext->RequestMemory != NULL) {
		WdfObjectDelete(requestContext->RequestMemory);
	}

	// Issue next request
	PtpFilterDiagnosticsInputIssueRequest(requestContext->DeviceContext->Device);
}

PCHAR
PtpFilterDiagnosticsIoControlGetString(
	_In_ ULONG IoControlCode
)
{
	switch (IoControlCode)
	{
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";
	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";
	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		return "IOCTL_HID_GET_REPORT_DESCRIPTOR";
	case IOCTL_HID_GET_STRING:
		return "IOCTL_HID_GET_STRING";
	case IOCTL_HID_READ_REPORT:
		return "IOCTL_HID_READ_REPORT";
	case IOCTL_HID_WRITE_REPORT:
		return "IOCTL_HID_WRITE_REPORT";
	case IOCTL_UMDF_HID_GET_INPUT_REPORT:
		return "IOCTL_UMDF_HID_GET_INPUT_REPORT";
	case IOCTL_UMDF_HID_SET_OUTPUT_REPORT:
		return "IOCTL_UMDF_HID_SET_OUTPUT_REPORT";
	case IOCTL_UMDF_HID_GET_FEATURE:
		return "IOCTL_UMDF_HID_GET_FEATURE";
	case IOCTL_UMDF_HID_SET_FEATURE:
		return "IOCTL_UMDF_HID_SET_FEATURE";
	case IOCTL_HID_ACTIVATE_DEVICE:
		return "IOCTL_HID_ACTIVATE_DEVICE";
	case IOCTL_HID_DEACTIVATE_DEVICE:
		return "IOCTL_HID_DEACTIVATE_DEVICE";
	case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
		return "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST";
	case IOCTL_HID_GET_FEATURE:
		return "IOCTL_HID_GET_FEATURE";
	case IOCTL_HID_SET_FEATURE:
		return "IOCTL_HID_SET_FEATURE";
	default:
		return "IOCTL_UNKNOWN";
	}
}
