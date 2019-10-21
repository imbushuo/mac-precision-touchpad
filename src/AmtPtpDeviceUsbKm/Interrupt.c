// Interrupt.c: Handles device input event

#include "Driver.h"
#include "Interrupt.tmh"

// Helper function for numberic operation
static inline INT AmtRawToInteger(
	_In_ USHORT x
)
{
	return (signed short)x;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpConfigContReaderForInterruptEndPoint(
	_In_ PDEVICE_CONTEXT DeviceContext
)
{
	WDF_USB_CONTINUOUS_READER_CONFIG contReaderConfig;
	NTSTATUS status;
	size_t transferLength = 0;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	switch (DeviceContext->DeviceInfo->tp_type)
	{
	case TYPE1:
		transferLength = HEADER_TYPE1 + FSIZE_TYPE1 * MAX_FINGERS;
		break;
	case TYPE2:
		transferLength = HEADER_TYPE2 + FSIZE_TYPE2 * MAX_FINGERS;
		break;
	case TYPE3:
		transferLength = HEADER_TYPE3 + FSIZE_TYPE3 * MAX_FINGERS;
		break;
	case TYPE4:
		transferLength = HEADER_TYPE4 + FSIZE_TYPE4 * MAX_FINGERS;
		break;
	case TYPE5:
		transferLength = HEADER_TYPE5 + FSIZE_TYPE5 * MAX_FINGERS;
		break;
	}

	if (transferLength <= 0) {
		status = STATUS_UNKNOWN_REVISION;
		goto exit;
	}

	WDF_USB_CONTINUOUS_READER_CONFIG_INIT(
		&contReaderConfig,
		AmtPtpEvtUsbInterruptPipeReadComplete,
		DeviceContext,		// Context
		transferLength		// Calculate transferred length by device information
	);

	contReaderConfig.EvtUsbTargetPipeReadersFailed = AmtPtpEvtUsbInterruptReadersFailed;

	// Remember to turn it on in D0 entry
	status = WdfUsbTargetPipeConfigContinuousReader(
		DeviceContext->InterruptPipe,
		&contReaderConfig
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! AmtPtpConfigContReaderForInterruptEndPoint failed with Status code %!STATUS!",
			status
		);
		goto exit;
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return STATUS_SUCCESS;
}

VOID
AmtPtpEvtUsbInterruptPipeReadComplete(
	_In_ WDFUSBPIPE  Pipe,
	_In_ WDFMEMORY   Buffer,
	_In_ size_t      NumBytesTransferred,
	_In_ WDFCONTEXT  Context
)
{
	UNREFERENCED_PARAMETER(Pipe);

	PDEVICE_CONTEXT pDeviceContext = Context;
	size_t headerSize = (unsigned int)pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int)pDeviceContext->DeviceInfo->tp_fsize;
	size_t raw_n, i;
	USHORT x = 0, y = 0;
	UCHAR* TouchBuffer = NULL;
	const struct TRACKPAD_FINGER* f = NULL;

	LONGLONG PerfCounterDelta;
	LARGE_INTEGER CurrentPerfCounter;
	NTSTATUS Status;
	PTP_REPORT PtpReport;

	WDFREQUEST Request;
	WDFMEMORY  RequestMemory;

	if (NumBytesTransferred < headerSize || (NumBytesTransferred - headerSize) % fingerprintSize != 0) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! Malformed input received. Length = %llu",
			NumBytesTransferred
		);
		return;
	}

	// Retrieve packet
	TouchBuffer = WdfMemoryGetBuffer(
		Buffer,
		NULL
	);

	if (TouchBuffer == NULL) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"%!FUNC! Failed to retrieve packet"
		);
		return;
	}

	// Retrieve next PTP touchpad request.
	Status = WdfIoQueueRetrieveNextRequest(
		pDeviceContext->InputQueue,
		&Request
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"%!FUNC! No pending PTP request. Disposed"
		);
		return;
	}

	Status = WdfRequestRetrieveOutputMemory(
		Request,
		&RequestMemory
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputMemory failed with %!STATUS!",
			Status
		);
		return;
	}

	// Prepare report
	Status = STATUS_SUCCESS;

	RtlZeroMemory(&PtpReport, sizeof(PTP_REPORT));
	PtpReport.ReportID = REPORTID_MULTITOUCH;
	PtpReport.IsButtonClicked = 0;
	raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
	UCHAR* f_base = TouchBuffer + headerSize + pDeviceContext->DeviceInfo->tp_delta;

	// Scan time is in 100us
	KeQueryPerformanceCounter(&CurrentPerfCounter);
	PerfCounterDelta = (CurrentPerfCounter.QuadPart - pDeviceContext->LastReportTime.QuadPart) / 100;
	if (PerfCounterDelta > 0xFF) {
		PerfCounterDelta = 0xFF;
	}

	PtpReport.ScanTime = (USHORT) PerfCounterDelta;

	if (pDeviceContext->PtpReportTouch) {
		if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;
		if (raw_n * fingerprintSize < (NumBytesTransferred - headerSize)) {
			TraceEvents(
				TRACE_LEVEL_ERROR, TRACE_DRIVER,
				"%!FUNC! Buffer may have a problem"
			);
			WdfRequestComplete(Request, STATUS_DATA_ERROR);
			return;
		}

		PtpReport.ContactCount = (UCHAR) raw_n;

		for (i = 0; i < raw_n; i++) {
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);
			
			// Translate X and Y
			x = (AmtRawToInteger(f->abs_x) - pDeviceContext->DeviceInfo->x.min) > 0 ? 
				((USHORT)(AmtRawToInteger(f->abs_x) - pDeviceContext->DeviceInfo->x.min)) : 0;
			y = (pDeviceContext->DeviceInfo->y.max - AmtRawToInteger(f->abs_y)) > 0 ? 
				((USHORT)(pDeviceContext->DeviceInfo->y.max - AmtRawToInteger(f->abs_y))) : 0;

			// Defuzz functions remain the same
			// TODO: Implement defuzz later
			PtpReport.Contacts[i].ContactID = (UCHAR) i;
			PtpReport.Contacts[i].X = x;
			PtpReport.Contacts[i].Y = y;
			PtpReport.Contacts[i].TipSwitch = (AmtRawToInteger(f->touch_major) << 1) >= 200;
			PtpReport.Contacts[i].Confidence = (AmtRawToInteger(f->touch_minor) << 1) > 0;
		}
	}

	if (pDeviceContext->PtpReportButton) {
		// Handles trackpad button input here.
		if (TouchBuffer[pDeviceContext->DeviceInfo->tp_button]) {
			PtpReport.IsButtonClicked = TRUE;
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_INPUT,
				"%!FUNC!: Trackpad button clicked"
			);
		}
	}

	// Compose final report and write it back
	Status = WdfMemoryCopyFromBuffer(
		RequestMemory,
		0,
		(PVOID) &PtpReport,
		sizeof(PTP_REPORT)
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
			Status
		);
		return;
	}

	// Set result
	WdfRequestSetInformation(Request, sizeof(PTP_REPORT));

	// Set completion flag
	WdfRequestComplete(Request, Status);
}

BOOLEAN
AmtPtpEvtUsbInterruptReadersFailed(
	_In_ WDFUSBPIPE Pipe,
	_In_ NTSTATUS Status,
	_In_ USBD_STATUS UsbdStatus
)
{
	UNREFERENCED_PARAMETER(Pipe);
	UNREFERENCED_PARAMETER(UsbdStatus);
	UNREFERENCED_PARAMETER(Status);

	return TRUE;
}
