// InputInterrupt.c: Handles device input event

#include "Driver.h"
#include "InputInterrupt.tmh"

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
		return status;
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
		return status;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return STATUS_SUCCESS;

}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpEvtUsbInterruptPipeReadComplete(
	_In_ WDFUSBPIPE  Pipe,
	_In_ WDFMEMORY   Buffer,
	_In_ size_t      NumBytesTransferred,
	_In_ WDFCONTEXT  Context
)
{
	UNREFERENCED_PARAMETER(Pipe);

	WDFDEVICE       device;
	PDEVICE_CONTEXT pDeviceContext = Context;
	UCHAR*			pBuffer = NULL;
	NTSTATUS        status;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	device = WdfObjectContextGetObject(pDeviceContext);
	size_t headerSize = (unsigned int) pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) pDeviceContext->DeviceInfo->tp_fsize;

	if (NumBytesTransferred < headerSize || (NumBytesTransferred - headerSize) % fingerprintSize != 0) {

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! Malformed input received. Length = %llu. Attempt to reset device.",
			NumBytesTransferred
		);

		status = AmtPtpEmergResetDevice(pDeviceContext);
		if (!NT_SUCCESS(status)) {

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! AmtPtpEmergResetDevice failed with %!STATUS!",
				status
			);

		}

		return;
	}

	if (!pDeviceContext->IsWellspringModeOn) {

		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_DRIVER,
			"%!FUNC! Routine is called without enabling Wellspring mode"
		);

		return;
	}

	// Dispatch USB Interrupt routine by device family
	switch (pDeviceContext->DeviceInfo->tp_type) {
		case TYPE1:
		{
			TraceEvents(
				TRACE_LEVEL_WARNING,
				TRACE_DRIVER,
				"%!FUNC! Mode not yet supported"
			);
			break;
		}
		// Universal routine handler
		case TYPE2:
		case TYPE3:
		case TYPE4:
		{
			pBuffer = WdfMemoryGetBuffer(
				Buffer,
				NULL
			);

			status = AmtPtpServiceTouchInputInterrupt(
				pDeviceContext,
				pBuffer,
				NumBytesTransferred
			);

			if (!NT_SUCCESS(status)) {
				TraceEvents(
					TRACE_LEVEL_WARNING,
					TRACE_DRIVER,
					"%!FUNC! AmtPtpServiceTouchInputInterrupt failed with %!STATUS!",
					status
				);
			}
			break;
		}
		// Magic Trackpad 2
		case TYPE5:
		{
			pBuffer = WdfMemoryGetBuffer(
				Buffer,
				NULL
			);
			status = AmtPtpServiceTouchInputInterruptType5(
				pDeviceContext,
				pBuffer,
				NumBytesTransferred
			);

			if (!NT_SUCCESS(status)) {
				TraceEvents(
					TRACE_LEVEL_WARNING,
					TRACE_DRIVER,
					"%!FUNC! AmtPtpServiceTouchInputInterrupt5 failed with %!STATUS!",
					status
				);
			}
			break;
		}
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

}

_IRQL_requires_(PASSIVE_LEVEL)
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

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpServiceTouchInputInterrupt(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
)
{
	NTSTATUS Status;
	WDFREQUEST Request;
	WDFMEMORY  RequestMemory;
	PTP_REPORT PtpReport;
	LARGE_INTEGER CurrentPerfCounter;
	LONGLONG PerfCounterDelta;

	const struct TRACKPAD_FINGER *f;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int) DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) DeviceContext->DeviceInfo->tp_fsize;
	USHORT x = 0, y = 0;

	Status = STATUS_SUCCESS;
	PtpReport.ReportID = REPORTID_MULTITOUCH;
	PtpReport.IsButtonClicked = 0;

	// Retrieve next PTP touchpad request.
	Status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->InputQueue,
		&Request
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! No pending PTP request. Interrupt disposed"
		);
		goto exit;
	}

	QueryPerformanceCounter(
		&CurrentPerfCounter
	);

	// Scan time is in 100us
	PerfCounterDelta = (CurrentPerfCounter.QuadPart - DeviceContext->PerfCounter.QuadPart) / 100;
	// Only two bytes allocated
	if (PerfCounterDelta > 0xFF)
	{
		PerfCounterDelta = 0xFF;
	}

	PtpReport.ScanTime = (USHORT) PerfCounterDelta;

	// Allocate output memory.
	Status = WdfRequestRetrieveOutputMemory(
		Request,
		&RequestMemory
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputMemory failed with %!STATUS!",
			Status
		);
		goto exit;
	}

	// Type 2 touchpad surface report
	if (DeviceContext->IsSurfaceReportOn) {
		// Handles trackpad surface report here.
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;
		PtpReport.ContactCount = (UCHAR) raw_n;

#ifdef INPUT_CONTENT_TRACE
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! with %llu points.",
			raw_n
		);
#endif

		// Fingers
		for (i = 0; i < raw_n; i++) {

			UCHAR *f_base = Buffer + headerSize + DeviceContext->DeviceInfo->tp_delta;
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);

			// Translate X and Y
			x = (AmtRawToInteger(f->abs_x) - DeviceContext->DeviceInfo->x.min) > 0 ? 
				((USHORT)(AmtRawToInteger(f->abs_x) - DeviceContext->DeviceInfo->x.min)) : 0;
			y = (DeviceContext->DeviceInfo->y.max - AmtRawToInteger(f->abs_y)) > 0 ? 
				((USHORT)(DeviceContext->DeviceInfo->y.max - AmtRawToInteger(f->abs_y))) : 0;

			// Defuzz functions remain the same
			// TODO: Implement defuzz later
			PtpReport.Contacts[i].ContactID = (UCHAR) i;
			PtpReport.Contacts[i].X = x;
			PtpReport.Contacts[i].Y = y;
			PtpReport.Contacts[i].TipSwitch = (AmtRawToInteger(f->touch_major) << 1) >= 200;
			PtpReport.Contacts[i].Confidence = (AmtRawToInteger(f->touch_minor) << 1) > 0;

#ifdef INPUT_CONTENT_TRACE
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_INPUT,
				"%!FUNC!: Point %llu, X = %d, Y = %d, TipSwitch = %d, Confidence = %d, tMajor = %d, tMinor = %d, origin = %d, PTP Origin = %d",
				i,
				PtpReport.Contacts[i].X,
				PtpReport.Contacts[i].Y,
				PtpReport.Contacts[i].TipSwitch,
				PtpReport.Contacts[i].Confidence,
				AmtRawToInteger(f->touch_major) << 1,
				AmtRawToInteger(f->touch_minor) << 1,
				AmtRawToInteger(f->origin),
				(UCHAR) i
			);
#endif
		}
	}

	// Type 2 touchpad contains integrated trackpad buttons
	if (DeviceContext->IsButtonReportOn) {
		// Handles trackpad button input here.
		if (Buffer[DeviceContext->DeviceInfo->tp_button]) {
			PtpReport.IsButtonClicked = TRUE;
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
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
			Status
		);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(
		Request,
		sizeof(PTP_REPORT)
	);

	// Set completion flag
	WdfRequestComplete(
		Request,
		Status
	);

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);
	return Status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpServiceTouchInputInterruptType5(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
)
{

	NTSTATUS   Status;
	WDFREQUEST Request;
	WDFMEMORY  RequestMemory;
	PTP_REPORT PtpReport;
	LARGE_INTEGER CurrentPerfCounter;
	LONGLONG PerfCounterDelta;

	const struct TRACKPAD_FINGER *f;
	const struct TRACKPAD_FINGER_TYPE5 *f_type5;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	Status = STATUS_SUCCESS;
	PtpReport.ReportID = REPORTID_MULTITOUCH;
	PtpReport.IsButtonClicked = 0;

	INT x, y = 0;
	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int) DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) DeviceContext->DeviceInfo->tp_fsize;

	Status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->InputQueue,
		&Request
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, 
			TRACE_DRIVER, 
			"%!FUNC! No pending PTP request. Interrupt disposed"
		);
		goto exit;
	}

	Status = WdfRequestRetrieveOutputMemory(
		Request, 
		&RequestMemory
	);
	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", 
			Status
		);
		goto exit;
	}

	QueryPerformanceCounter(
		&CurrentPerfCounter
	);

	// Scan time is in 100us
	PerfCounterDelta = (CurrentPerfCounter.QuadPart - DeviceContext->PerfCounter.QuadPart) / 100;
	// Only two bytes allocated
	if (PerfCounterDelta > 0xFF)
	{
		PerfCounterDelta = 0xFF;
	}

	PtpReport.ScanTime = (USHORT) PerfCounterDelta;

	// Type 5 finger report
	if (DeviceContext->IsSurfaceReportOn) {
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;
		PtpReport.ContactCount = (UCHAR)raw_n;

#ifdef INPUT_CONTENT_TRACE
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! with %llu points.",
			raw_n
		);
#endif

		// Fingers to array
		for (i = 0; i < raw_n; i++) {

			UCHAR *f_base = Buffer + headerSize + DeviceContext->DeviceInfo->tp_delta;
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);
			f_type5 = (const struct TRACKPAD_FINGER_TYPE5*) f;

			USHORT tmp_x = (*((USHORT*)f_type5)) & 0x1fff;
			UINT tmp_y = (INT)(*((UINT*)f_type5));

			x = (SHORT) (tmp_x << 3) >> 3;
			y = -(INT) (tmp_y << 6) >> 19;

			x = (x - DeviceContext->DeviceInfo->x.min) > 0 ? (x - DeviceContext->DeviceInfo->x.min) : 0;
			y = (y - DeviceContext->DeviceInfo->y.min) > 0 ? (y - DeviceContext->DeviceInfo->y.min) : 0;

			PtpReport.Contacts[i].ContactID = f_type5->ContactIdentifier.Id;
			PtpReport.Contacts[i].X = (USHORT) x;
			PtpReport.Contacts[i].Y = (USHORT) y;
			PtpReport.Contacts[i].TipSwitch = (AmtRawToInteger(f_type5->TouchMajor) << 1) > 0;

			// The Microsoft spec says reject any input larger than 25mm. This is not ideal
			// for Magic Trackpad 2 - so we raised the threshold a bit higher.
			// Or maybe I used the wrong unit? IDK
			PtpReport.Contacts[i].Confidence = (AmtRawToInteger(f_type5->TouchMinor) << 1) < 345 && 
				(AmtRawToInteger(f_type5->TouchMinor) << 1) < 345;

#ifdef INPUT_CONTENT_TRACE
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_INPUT,
				"%!FUNC!: Point %llu, X = %d, Y = %d, TipSwitch = %d, Confidence = %d, tMajor = %d, tMinor = %d, origin = %d",
				i,
				PtpReport.Contacts[i].X,
				PtpReport.Contacts[i].Y,
				PtpReport.Contacts[i].TipSwitch,
				PtpReport.Contacts[i].Confidence,
				AmtRawToInteger(f_type5->TouchMajor) << 1,
				AmtRawToInteger(f_type5->TouchMinor) << 1,
				f_type5->ContactIdentifier.Id
			);
#endif
		}
	}

	// Button
	if (DeviceContext->IsButtonReportOn) {
		if (Buffer[DeviceContext->DeviceInfo->tp_button]) {
			PtpReport.IsButtonClicked = TRUE;
		}
	}

	// Write output
	Status = WdfMemoryCopyFromBuffer(
		RequestMemory, 
		0, 
		(PVOID) &PtpReport, 
		sizeof(PTP_REPORT)
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", 
			Status
		);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(
		Request, 
		sizeof(PTP_REPORT)
	);

	// Set completion flag
	WdfRequestComplete(
		Request, 
		Status
	);

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return Status;

}

// Helper function for numberic operation
static inline INT AmtRawToInteger(
	_In_ USHORT x
)
{
	return (signed short) x;
}
