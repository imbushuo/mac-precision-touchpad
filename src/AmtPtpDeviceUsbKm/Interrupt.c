// Interrupt.c: Handles device input event

#include "Driver.h"
#include "Interrupt.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AmtPtpConfigContReaderForInterruptEndPoint)
#pragma alloc_text (PAGE, AmtPtpEvtUsbInterruptPipeReadComplete)
#pragma alloc_text (PAGE, AmtPtpEvtUsbInterruptReadersFailed)
#endif

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
	UNREFERENCED_PARAMETER(Buffer);

	PDEVICE_CONTEXT pDeviceContext = Context;
	size_t headerSize = (unsigned int) pDeviceContext->DeviceInfo->tp_header;
	size_t fingerprintSize = (unsigned int) pDeviceContext->DeviceInfo->tp_fsize;
	size_t raw_n, i;
	UCHAR* szBuffer = NULL;
	const struct TRACKPAD_FINGER* f = NULL;

	if (NumBytesTransferred < headerSize || (NumBytesTransferred - headerSize) % fingerprintSize != 0) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! Malformed input received. Length = %llu",
			NumBytesTransferred
		);
		return;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Transferred packet size %lld",
		NumBytesTransferred
	);

	szBuffer = WdfMemoryGetBuffer(
		Buffer,
		NULL
	);

	if (szBuffer != NULL) {
		KeQueryPerformanceCounter(&pDeviceContext->LastReportTime);
		raw_n = (NumBytesTransferred - headerSize) / fingerprintSize;
		UCHAR* f_base = szBuffer + headerSize + pDeviceContext->DeviceInfo->tp_delta;
		for (i = 0; i < raw_n; i++) {
			f = (const struct TRACKPAD_FINGER*) (f_base + i * fingerprintSize);
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"Finger %lld AbsX %d AbsY %d TMaj %d TMin %d Origin %d",
				i,
				AmtRawToInteger(f->abs_x),
				AmtRawToInteger(f->abs_y),
				AmtRawToInteger(f->touch_major) << 1,
				AmtRawToInteger(f->touch_minor) << 1,
				f->origin
			);
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
	UNREFERENCED_PARAMETER(Pipe);
	UNREFERENCED_PARAMETER(Status);
	UNREFERENCED_PARAMETER(UsbdStatus);

	return TRUE;
}
