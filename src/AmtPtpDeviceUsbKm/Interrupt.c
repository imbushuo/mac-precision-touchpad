// Interrupt.c: Handles device input event

#include "Driver.h"
#include "Interrupt.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AmtPtpConfigContReaderForInterruptEndPoint)
#pragma alloc_text (PAGE, AmtPtpEvtUsbInterruptPipeReadComplete)
#pragma alloc_text (PAGE, AmtPtpEvtUsbInterruptReadersFailed)
#endif

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
	UNREFERENCED_PARAMETER(NumBytesTransferred);
	UNREFERENCED_PARAMETER(Context);
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
