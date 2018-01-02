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
	UCHAR*			szBuffer = NULL;
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

	if (!pDeviceContext->IsWellspringModeOn) return;

	// Dispatch USB Interrupt routine by device family
	switch (pDeviceContext->DeviceInfo->tp_type) {
			case TYPE1:
			case TYPE4:
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
			{

				szBuffer = WdfMemoryGetBuffer(
					Buffer,
					NULL
				);
				status = AmtPtpServiceTouchInputInterrupt(
					pDeviceContext,
					szBuffer,
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

				szBuffer = WdfMemoryGetBuffer(
					Buffer,
					NULL
				);
				status = AmtPtpServiceTouchInputInterruptType5(
					pDeviceContext,
					szBuffer,
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
	WDFDEVICE device = WdfIoTargetGetDevice(WdfUsbTargetPipeGetIoTarget(Pipe));

	UNREFERENCED_PARAMETER(device);
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
	
	NTSTATUS status;
	WDFREQUEST request;
	WDFMEMORY  reqMemory;
	PTP_REPORT report;
	const struct TRACKPAD_FINGER *f;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int)DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int)DeviceContext->DeviceInfo->tp_fsize;
	USHORT x = 0, y = 0;

	status = STATUS_SUCCESS;
	report.ReportID = REPORTID_MULTITOUCH;
	report.ScanTime = 10000;
	report.IsButtonClicked = 0;

	// Retrieve next PTP touchpad request.
	status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->InputQueue,
		&request
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! No pending PTP request. Interrupt disposed"
		);
		goto exit;
	}

	// Allocate output memory.
	status = WdfRequestRetrieveOutputMemory(
		request,
		&reqMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputMemory failed with %!STATUS!",
			status
		);
		goto exit;
	}

	// Type 2 touchpad surface report
	if (DeviceContext->IsSurfaceReportOn) {
		// Handles trackpad surface report here.
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;
		report.ContactCount = (UCHAR) raw_n;

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! with %llu points.",
			raw_n
		);

		// Fingers
		for (i = 0; i < raw_n; i++) {

			UCHAR *f_base = Buffer + headerSize + DeviceContext->DeviceInfo->tp_delta;
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);

			// Translate X and Y
			x = (USHORT) (AmtRawToInteger(f->abs_x) - DeviceContext->DeviceInfo->x.min);
			y = (USHORT) (DeviceContext->DeviceInfo->y.max - AmtRawToInteger(f->abs_y));

			// Defuzz functions remain the same
			// TODO: Implement defuzz later
			report.Contacts[i].ContactID = (UCHAR) i;
			report.Contacts[i].X = x;
			report.Contacts[i].Y = y;
			report.Contacts[i].TipSwitch = (AmtRawToInteger(f->touch_major) << 1) >= 200;
			report.Contacts[i].Confidence = (AmtRawToInteger(f->touch_minor) << 1) > 0;

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_INPUT,
				"%!FUNC!: Point %llu, X = %d, Y = %d, TipSwitch = %d, Confidence = %d, tMajor = %d, tMinor = %d, origin = %d, PTP Origin = %d",
				i,
				report.Contacts[i].X,
				report.Contacts[i].Y,
				report.Contacts[i].TipSwitch,
				report.Contacts[i].Confidence,
				AmtRawToInteger(f->touch_major) << 1,
				AmtRawToInteger(f->touch_minor) << 1,
				AmtRawToInteger(f->origin),
				(UCHAR) i
			);

		}
	}

	// Type 2 touchpad contains integrated trackpad buttons
	if (DeviceContext->IsButtonReportOn) {
		// Handles trackpad button input here.
		if (Buffer[DeviceContext->DeviceInfo->tp_button]) {
			report.IsButtonClicked = TRUE;
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_INPUT,
				"%!FUNC!: Trackpad button clicked"
			);
		}
	}

	// Compose final report and write it back
	status = WdfMemoryCopyFromBuffer(
		reqMemory,
		0,
		(PVOID) &report,
		sizeof(PTP_REPORT)
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
			status
		);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(
		request,
		sizeof(PTP_REPORT)
	);

	// Set completion flag
	WdfRequestComplete(
		request,
		status
	);

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpServiceTouchInputInterruptType5(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
)
{

	NTSTATUS   status;
	WDFREQUEST request;
	WDFMEMORY  reqMemory;
	PTP_REPORT report;

	const struct TRACKPAD_FINGER *f;
	const struct TRACKPAD_FINGER_TYPE5 *f_type5;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = STATUS_SUCCESS;
	INT x, y = 0;

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int) DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) DeviceContext->DeviceInfo->tp_fsize;
	UCHAR actualFingers = 0;
	UCHAR muTotalPressure = 0;
	UCHAR muTotalSize = 0;

	status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->InputQueue,
		&request
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, 
			TRACE_DRIVER, 
			"%!FUNC! No pending PTP request. Interrupt disposed"
		);
		goto exit;
	}

	status = WdfRequestRetrieveOutputMemory(
		request, 
		&reqMemory
	);
	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", 
			status
		);
		goto exit;
	}

	// First things
	report.ReportID = REPORTID_MULTITOUCH;
	report.ScanTime = 10000;
	report.IsButtonClicked = 0;

	// Check things to report
	if (DeviceContext->IsSurfaceReportOn) {
		// Iterations to read
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;

		// Fingers
		for (i = 0; i < raw_n; i++) {
			UCHAR *f_base = Buffer + headerSize + DeviceContext->DeviceInfo->tp_delta;
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);
			f_type5 = (const struct TRACKPAD_FINGER_TYPE5*) f;

			USHORT tmp_x;
			UINT tmp_y;
			tmp_x = (*((USHORT*)f_type5)) & 0x1fff;
			tmp_y = (INT) (*((UINT*)f_type5));

			x = (SHORT) (tmp_x << 3) >> 3;
			y = -(INT) (tmp_y << 6) >> 19;

			// We need to defuzz input
			if (DeviceContext->ContactRepository[i].ContactId == f_type5->ContactIdentifier.Id) {
				DeviceContext->ContactRepository[i].X = (USHORT) AmtPtpDefuzzInput(
					x, 
					DeviceContext->ContactRepository[i].X, 
					DeviceContext->HorizonalFuzz
				);
				DeviceContext->ContactRepository[i].Y = (USHORT) AmtPtpDefuzzInput(
					y, 
					DeviceContext->ContactRepository[i].Y, 
					DeviceContext->VerticalFuzz
				);
				DeviceContext->ContactRepository[i].Pressure = (UCHAR) AmtPtpDefuzzInput(
					f_type5->Pressure, 
					DeviceContext->ContactRepository[i].Pressure, 
					DeviceContext->PressureFuzz
				);
				DeviceContext->ContactRepository[i].Size = (UCHAR) AmtPtpDefuzzInput(
					f_type5->Size, 
					DeviceContext->ContactRepository[i].Size, 
					DeviceContext->WidthFuzz
				);
				DeviceContext->ContactRepository[i].Orientation = (USHORT)AmtPtpDefuzzInput(
					MAX_FINGER_ORIENTATION - ((f_type5->RawOrientationAndOrigin & 0xf0) << 6),
					DeviceContext->ContactRepository[i].Orientation,
					DeviceContext->OrientationFuzz
				);
			} else {
				DeviceContext->ContactRepository[i].ContactId = f_type5->ContactIdentifier.Id;
				DeviceContext->ContactRepository[i].X = (USHORT) x;
				DeviceContext->ContactRepository[i].Y = (USHORT) y;
				DeviceContext->ContactRepository[i].Pressure = f_type5->Pressure;
				DeviceContext->ContactRepository[i].Size = f_type5->Size;
				DeviceContext->ContactRepository[i].Orientation = MAX_FINGER_ORIENTATION - ((f_type5->RawOrientationAndOrigin & 0xf0) << 6);
			}

			// These two values don't get defuzzed
			DeviceContext->ContactRepository[i].TouchMajor = f_type5->TouchMajor << 2;
			DeviceContext->ContactRepository[i].TouchMinor = f_type5->TouchMinor << 2;

			// Set ID.
			report.Contacts[i].ContactID = f_type5->ContactIdentifier.Id;

			// Adjust position. Apple uses different coordinate system.
			report.Contacts[i].X = (USHORT) (DeviceContext->ContactRepository[i].X - DeviceContext->DeviceInfo->x.min);
			report.Contacts[i].Y = (USHORT) (DeviceContext->ContactRepository[i].Y - DeviceContext->DeviceInfo->y.min);

			// Set flags (by cases)
			if (raw_n == 1) {
				report.Contacts[i].TipSwitch = DeviceContext->ContactRepository[i].TouchMajor * DeviceContext->ContactRepository[i].TouchMinor > 78000;
				// report.Contacts[i].TipSwitch = DeviceContext->ContactRepository[i].Pressure > DeviceContext->PressureQualLevel;
				report.Contacts[i].Confidence = DeviceContext->ContactRepository[i].Size >= DeviceContext->SgContactSizeQualLevel;

				TraceEvents(
					TRACE_LEVEL_INFORMATION, 
					TRACE_INPUT,
					"(SG) Finger %d, X: %d, Y: %d, O: %d, TMajor: %d, TMinor: %d, Pressure: %d, Size: %d, TipSwitch: %d, Confidence: %d",
					f_type5->ContactIdentifier.Id,
					report.Contacts[i].X,
					report.Contacts[i].Y,
					DeviceContext->ContactRepository[i].Orientation,
					DeviceContext->ContactRepository[i].TouchMajor,
					DeviceContext->ContactRepository[i].TouchMinor,
					DeviceContext->ContactRepository[i].Pressure,
					DeviceContext->ContactRepository[i].Size,
					report.Contacts[i].TipSwitch,
					report.Contacts[i].Confidence
				);
			} else {

				// Save the information
				// Use size to determine confidence in MU scenario
				muTotalPressure += DeviceContext->ContactRepository[i].Pressure;
				muTotalSize += DeviceContext->ContactRepository[i].Size;
				
				report.Contacts[i].TipSwitch = DeviceContext->ContactRepository[i].TouchMajor * DeviceContext->ContactRepository[i].TouchMinor > 78000;
				// report.Contacts[i].TipSwitch = DeviceContext->ContactRepository[i].Pressure > DeviceContext->PressureQualLevel;
				report.Contacts[i].Confidence = DeviceContext->ContactRepository[i].Size >= DeviceContext->MuContactSizeQualLevel;

				TraceEvents(
					TRACE_LEVEL_INFORMATION, 
					TRACE_INPUT,
					"(MU) Finger %d, X: %d, Y: %d, O: %d, TMajor: %d, TMinor: %d, Pressure: %d, Size: %d, TipSwitch: %d, Confidence: %d",
					f_type5->ContactIdentifier.Id,
					report.Contacts[i].X,
					report.Contacts[i].Y,
					DeviceContext->ContactRepository[i].Orientation,
					DeviceContext->ContactRepository[i].TouchMajor,
					DeviceContext->ContactRepository[i].TouchMinor,
					DeviceContext->ContactRepository[i].Pressure,
					DeviceContext->ContactRepository[i].Size,
					report.Contacts[i].TipSwitch,
					report.Contacts[i].Confidence
				);
			}
		
			actualFingers++;
		}

		if (actualFingers > 2) {
			if (muTotalPressure > DeviceContext->PressureQualLevel * 2.15) {
				TraceEvents(
					TRACE_LEVEL_INFORMATION,
					TRACE_INPUT,
					"(MU) Perform finger tip switch bit correction."
				);
				for (i = 0; i < actualFingers; i++) {
					report.Contacts[i].TipSwitch = 1;
				}

				if (muTotalSize > DeviceContext->MuContactSizeQualLevel * 2.15) {
					TraceEvents(
						TRACE_LEVEL_INFORMATION,
						TRACE_INPUT,
						"(MU) Perform finger confidence bit correction."
					);
					for (i = 0; i < actualFingers; i++) {
						report.Contacts[i].Confidence = 1;
					}
				}
			}
		}

		// Set header information
		report.ContactCount = actualFingers;

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_INPUT,
			"%!FUNC! With %d fingers",
			report.ContactCount
		);
	}

	// Button
	if (DeviceContext->IsButtonReportOn) {
		if (Buffer[DeviceContext->DeviceInfo->tp_button] && actualFingers < 2) {
			report.IsButtonClicked = TRUE;
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_INPUT,
				"%!FUNC!: Trackpad button clicked"
			);
		}
	}

	// Write output
	status = WdfMemoryCopyFromBuffer(
		reqMemory, 
		0, 
		(PVOID) &report, 
		sizeof(PTP_REPORT)
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", 
			status
		);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(
		request, 
		sizeof(PTP_REPORT)
	);

	// Set completion flag
	WdfRequestComplete(
		request, 
		status
	);

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;

}

// Debuzz function from Linux Kernel: drivers/input/input.c
_IRQL_requires_(PASSIVE_LEVEL)
static INT AmtPtpDefuzzInput(
	_In_ int NewValue,
	_In_ int OldValue,
	_In_ double Fuzz
)
{

	if (Fuzz) {
		if (NewValue > OldValue - Fuzz / 2 && NewValue < OldValue + Fuzz / 2)
			return OldValue;

		if (NewValue > OldValue - Fuzz && NewValue < OldValue + Fuzz)
			return (OldValue * 3 + NewValue) / 4;

		if (NewValue > OldValue - Fuzz * 2 && NewValue < OldValue + Fuzz * 2)
			return (OldValue + NewValue) / 2;
	}

	return NewValue;

}

// Helper function for numberic operation
static inline INT AmtRawToInteger(
	_In_ USHORT x
)
{
	return (signed short) x;
}