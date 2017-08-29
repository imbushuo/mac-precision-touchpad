// InputInterrupt.c: Handles device input event

#include "Driver.h"
#include "Types.h"
#include "AppleDefinition.h"
#include "InputInterrupt.tmh"

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2PtpDeviceConfigContReaderForInterruptEndPoint(
	_In_ PDEVICE_CONTEXT DeviceContext
)
{
	WDF_USB_CONTINUOUS_READER_CONFIG contReaderConfig;
	NTSTATUS status;

	WDF_USB_CONTINUOUS_READER_CONFIG_INIT(&contReaderConfig,
		MagicTrackpad2PtpDeviceEvtUsbInterruptPipeReadComplete,
		DeviceContext,    // Context
		HEADER_TYPE5 + FSIZE_TYPE5 * MAX_FINGERS);   // TransferLength

	contReaderConfig.EvtUsbTargetPipeReadersFailed = MagicTrackpad2PtpDeviceEvtUsbInterruptReadersFailed;

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
MagicTrackpad2PtpDeviceEvtUsbInterruptPipeReadComplete(
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

	device = WdfObjectContextGetObject(pDeviceContext);

	const struct TRACKPAD_FINGER *f;
	const struct TRACKPAD_FINGER_TYPE5 *f_type5;
	s32 x, y = 0;
	u16 pressure = 0;

	size_t raw_n, i = 0;
	size_t headerSize = (unsigned int)pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int)pDeviceContext->DeviceInfo->tp_fsize;

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
MagicTrackpad2PtpDeviceEvtUsbInterruptReadersFailed(
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
