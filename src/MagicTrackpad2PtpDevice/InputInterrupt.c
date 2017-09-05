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

	device = WdfObjectContextGetObject(pDeviceContext);

	size_t headerSize = (unsigned int) pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) pDeviceContext->DeviceInfo->tp_fsize;

	if (!pDeviceContext->IsWellspringModeOn) 
	{
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

_IRQL_requires_(PASSIVE_LEVEL)
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

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status = STATUS_SUCCESS;
	s32 x, y = 0;
	u16 pressure = 0;

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int)DeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int)DeviceContext->DeviceInfo->tp_fsize;
	UCHAR actualFingers = 0;

	status = WdfIoQueueRetrieveNextRequest(
		DeviceContext->InputQueue,
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
	if (raw_n >= PTP_MAX_CONTACT_POINTS) raw_n = PTP_MAX_CONTACT_POINTS;

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
		report.Contacts[i].Confidence = 1;
		report.Contacts[i].TipSwitch = 1;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"%!FUNC!: Finger %llu, x: %d, y: %d, pressure: %d\n", i, report.Contacts[i].X, report.Contacts[i].Y, pressure);

		actualFingers++;
	}

	// Set header information
	report.ContactCount = actualFingers;
	report.ReportID = REPORTID_MULTITOUCH;
	report.ScanTime = 14005;

	// Write output
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! With %d fingers.\n", report.ContactCount);
	status = WdfMemoryCopyFromBuffer(reqMemory, 0, (PVOID)&report, sizeof(PTP_REPORT));
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC!: WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		goto exit;
	}

	// Set result
	WdfRequestSetInformation(request, sizeof(PTP_REPORT));
	// Set completion flag
	WdfRequestComplete(request, status);

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}