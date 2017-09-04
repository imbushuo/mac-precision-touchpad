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

	WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&contReaderConfig,
		AmtPtpEvtUsbInterruptPipeReadComplete,
		DeviceContext,    // Context
		HEADER_TYPE5 + FSIZE_TYPE5 * MAX_FINGERS);   // TransferLength

	contReaderConfig.EvtUsbTargetPipeReadersFailed = AmtPtpEvtUsbInterruptReadersFailed;

	// Remember to turn it on in D0 entry
	status = WdfUsbTargetPipeConfigContinuousReader(DeviceContext->InterruptPipe,
		&contReaderConfig);

	if (!NT_SUCCESS(status)) {
		// TODO: Trace
		return status;
	}

	return STATUS_SUCCESS;
}

VOID
AmtPtpEvtUsbInterruptPipeReadComplete(
	WDFUSBPIPE  Pipe,
	WDFMEMORY   Buffer,
	size_t      NumBytesTransferred,
	WDFCONTEXT  Context
)
{
	UNREFERENCED_PARAMETER(Pipe);

	WDFDEVICE       device;
	PDEVICE_CONTEXT pDeviceContext = Context;
	UCHAR*			szBuffer = NULL;
	NTSTATUS        status;

	device = WdfObjectContextGetObject(pDeviceContext);

	size_t headerSize = (unsigned int) pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) pDeviceContext->DeviceInfo->tp_fsize;

	if (!pDeviceContext->IsWellspringModeOn && NumBytesTransferred == BCM5974_MOUSE_SIZE)
	{

		szBuffer = WdfMemoryGetBuffer(Buffer, NULL);

		// Dispatch to mouse routine
		status = AmtPtpServiceMouseInputInterrupt(pDeviceContext, szBuffer, NumBytesTransferred);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "%!FUNC!: AmtPtpServiceMouseInputInterrupt failed with %!STATUS!", status);
		}

		return;
	}

	if (pDeviceContext->DeviceInfo->tp_type != TYPE5)
	{
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "%!FUNC!: Mode not yet supported\n");
		return;
	}

	if (NumBytesTransferred < headerSize || (NumBytesTransferred - headerSize) % fingerprintSize != 0)
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"%!FUNC!: Malformed input received. Length = %llu\n", NumBytesTransferred);
	}
	else
	{
		// Dispatch to touch routine
		szBuffer = WdfMemoryGetBuffer(Buffer, NULL);
		status = AmtPtpServiceTouchInputInterruptType5(pDeviceContext, szBuffer, NumBytesTransferred);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, 
				"%!FUNC!: AmtPtpServiceTouchInputInterrupt failed with %!STATUS!", status);
		}
	}
}

BOOLEAN
AmtPtpEvtUsbInterruptReadersFailed(
	_In_ WDFUSBPIPE Pipe,
	_In_ NTSTATUS Status,
	_In_ USBD_STATUS UsbdStatus
)
{
	WDFDEVICE device = WdfIoTargetGetDevice(WdfUsbTargetPipeGetIoTarget(Pipe));
	// PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(device);

	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(UsbdStatus);
	UNREFERENCED_PARAMETER(Status);

	return TRUE;
}

NTSTATUS
AmtPtpServiceMouseInputInterrupt(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
)
{
	NTSTATUS   status;
	WDFREQUEST request;
	WDFMEMORY  reqMemory;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status   = STATUS_SUCCESS;
	request  = NULL;

	status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->MouseQueue,
		&request);

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC!: No pending mouse request. Interrupt disposed.");
		goto exit;
	}

	// Validate size
	if (NumBytesTransferred * sizeof(BYTE) != sizeof(HID_AAPL_MOUSE_REPORT))
	{
		status = STATUS_INVALID_BUFFER_SIZE;
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "%!FUNC!: Invalid mouse input. NumBytesTransferred = %llu, required size = %llu",
			NumBytesTransferred * sizeof(BYTE), sizeof(HID_AAPL_MOUSE_REPORT));
		goto exit;
	}

	// Simply forward input buffer to output. No other validation.
	status = WdfRequestRetrieveOutputMemory(request, &reqMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC!: WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		goto exit;
	}

	status = WdfMemoryCopyFromBuffer(reqMemory, 0, (PVOID)Buffer, NumBytesTransferred);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC!: WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		goto exit;
	}

	// Set information
	WdfRequestSetInformation(request, NumBytesTransferred);

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	// Set completion flag
	WdfRequestComplete(request, status);
	return status;
}

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

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status = STATUS_SUCCESS;
	s32 x, y = 0;
	u16 pressure = 0;

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int)DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int)DeviceContext->DeviceInfo->tp_fsize;

	status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->TouchQueue,
		&request);

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC!: No pending PTP request. Interrupt disposed.");
		goto exit;
	}

	status = WdfRequestRetrieveOutputMemory(request, &reqMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC!: WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		goto exit;
	}

	// Iterations to read
	raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
	// Set header information
	report.ContactCount = (UCHAR) raw_n;
	report.ReportID = REPORTID_MULTITOUCH;
	report.ScanTime = 39000;
	if (Buffer[DeviceContext->DeviceInfo->tp_button])
	{
		report.IsButtonClicked = TRUE;
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC!: Trackpad button clicked\n");
	}

	// Fingers
	for (i = 0; i < raw_n; i++)
	{
		u8 *f_base = Buffer + headerSize + DeviceContext->DeviceInfo->tp_delta;
		f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);

		u16 tmp_x;
		u32 tmp_y;
		f_type5 = (const struct TRACKPAD_FINGER_TYPE5*) f;

		tmp_x = (*((__le16*)f_type5)) & 0x1fff;
		tmp_y = (s32)(*((__le32*)f_type5));

		x = (s16)(tmp_x << 3) >> 3;
		y = -(s32)(tmp_y << 6) >> 19;
		pressure = f_type5->pressure;

		// Set ID.
		report.Contacts[i].ContactID = (UCHAR) i;

		// Adjust position. Apple uses different coordinate system.
		report.Contacts[i].X = (USHORT) (x - DeviceContext->DeviceInfo->x.min);
		report.Contacts[i].Y = (USHORT) (y - DeviceContext->DeviceInfo->y.min);

		// Don't know a better way to set this...
		if (pressure == 0)
		{
			report.Contacts[i].ContactStatus = 0;
		}
		else if (pressure > PRESSURE_LOWER_THRESHOLD)
		{
			report.Contacts[i].ContactStatus = PTP_CONTACT_CONFIDENCE_BIT | PTP_CONTACT_TIPSWITCH_BIT;
		}
		else
		{
			report.Contacts[i].ContactStatus = PTP_CONTACT_CONFIDENCE_BIT;
		}

		report.Contacts[i].Pressure = pressure;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"%!FUNC!: Finger %llu, x: %d, y: %d, pressure: %d\n", i, x, y, pressure);
	}

	// Write output
	status = WdfMemoryCopyFromBuffer(reqMemory, 0, (PVOID)&report, sizeof(PTP_REPORT));
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC!: WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(request, sizeof(PTP_REPORT));

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}