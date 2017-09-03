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

	const struct TRACKPAD_FINGER *f;
	const struct TRACKPAD_FINGER_TYPE5 *f_type5;

	s32 x, y = 0;
	u16 pressure = 0;

	size_t raw_n, i = 0;
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

	if (NumBytesTransferred < headerSize || (NumBytesTransferred - headerSize) % fingerprintSize != 0)
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
			"Malformed input received. Length = %llu\n", NumBytesTransferred);
	}
	else
	{
		// Iterations to read
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		szBuffer = WdfMemoryGetBuffer(Buffer, NULL);
		for (i = 0; i < raw_n; i++)
		{
			u8 *f_base = szBuffer + headerSize + pDeviceContext->DeviceInfo->tp_delta;
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);

			if (pDeviceContext->DeviceInfo->tp_type != TYPE5)
			{
				if (f->touch_major == 0) continue;
				x = f->abs_x;
				y = pDeviceContext->DeviceInfo->y.min + pDeviceContext->DeviceInfo->y.max - f->abs_y;
				pressure = f->pressure;
			}
			else
			{
				u16 tmp_x;
				u32 tmp_y;
				f_type5 = (const struct TRACKPAD_FINGER_TYPE5*) f;
				if (f_type5->pressure == 0) continue;
				
				// Modern Windows machines are all little-endian

				tmp_x = (*((__le16*)f_type5)) & 0x1fff;
				tmp_y = (s32)(*((__le32*)f_type5));

				x = (s16)(tmp_x << 3) >> 3;
				y = -(s32)(tmp_y << 6) >> 19;
				pressure = f_type5->pressure;
			}

			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"Finger %llu, x: %d, y: %d, pressure: %d\n", i, x, y, pressure);
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
AmtPtpServiceTouchInputInterrupt(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
)
{
	UNREFERENCED_PARAMETER(DeviceContext);
	UNREFERENCED_PARAMETER(Buffer);
	UNREFERENCED_PARAMETER(NumBytesTransferred);

	return STATUS_NOT_IMPLEMENTED;
}