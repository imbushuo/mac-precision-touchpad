// Queue.c: This file contains the queue entry points and callbacks.

#include <driver.h>
#include "queue.tmh"

NTSTATUS
AmtPtpDeviceQueueInitialize(
	_In_ WDFDEVICE Device
)
{

	WDFQUEUE queue;
	NTSTATUS status;
	WDF_IO_QUEUE_CONFIG    queueConfig;
	PDEVICE_CONTEXT	       deviceContext;

	deviceContext = DeviceGetContext(Device);

	//
	// Configure a default queue so that requests that are not
	// configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
	// other queues get dispatched here.
	//
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
		&queueConfig,
		WdfIoQueueDispatchParallel
	);

	queueConfig.EvtIoDeviceControl = AmtPtpDeviceEvtIoDeviceControl;
	queueConfig.EvtIoStop = AmtPtpDeviceEvtIoStop;

	status = WdfIoQueueCreate(
		Device,
		&queueConfig,
		WDF_NO_OBJECT_ATTRIBUTES,
		&queue
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_QUEUE, 
			"%!FUNC! WdfIoQueueCreate (Primary) failed %!STATUS!", 
			status
		);
		return status;
	}

	//
	// Create secondary queues for touch and mouse read requests.
	// 

	WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);
	queueConfig.PowerManaged = WdfFalse;

	status = WdfIoQueueCreate(
		Device,
		&queueConfig,
		WDF_NO_OBJECT_ATTRIBUTES,
		&deviceContext->InputQueue
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_QUEUE, 
			"%!FUNC! WdfIoQueueCreate (Input) failed %!STATUS!", 
			status
		);
		return status;
	}

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
PCHAR
DbgIoControlGetString(
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
	default:
		return "IOCTL_UNKNOWN";
	}

}

VOID
AmtPtpDeviceEvtIoDeviceControl(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t OutputBufferLength,
	_In_ size_t InputBufferLength,
	_In_ ULONG IoControlCode
)
{
	
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE device = WdfIoQueueGetDevice(Queue);
	BOOLEAN requestPending = FALSE;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_QUEUE,
		"%!FUNC!: Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d",
		Queue, 
		Request, 
		(int) OutputBufferLength, 
		(int) InputBufferLength, 
		IoControlCode
	);

	switch (IoControlCode)
	{
		case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
			status = AmtPtpGetHidDescriptor(
				device, 
				Request
			);
			break;
		case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
			status = AmtPtpGetDeviceAttribs(
				device, 
				Request
			);
			break;
		case IOCTL_HID_GET_REPORT_DESCRIPTOR:
			status = AmtPtpGetReportDescriptor(
				device, 
				Request
			);
			break;
		case IOCTL_HID_GET_STRING:
			status = AmtPtpGetStrings(
				device, 
				Request
			);
			break;
		case IOCTL_HID_READ_REPORT:
			status = AmtPtpDispatchReadReportRequests(
				device, 
				Request, 
				&requestPending
			);
			break;
		case IOCTL_UMDF_HID_GET_FEATURE:
			status = AmtPtpReportFeatures(
				device, 
				Request
			);
			break;
		case IOCTL_UMDF_HID_SET_FEATURE:
			status = AmtPtpSetFeatures(
				device, 
				Request
			);
			break;
		case IOCTL_HID_WRITE_REPORT:
		case IOCTL_UMDF_HID_SET_OUTPUT_REPORT:
		case IOCTL_UMDF_HID_GET_INPUT_REPORT:
		case IOCTL_HID_ACTIVATE_DEVICE:
		case IOCTL_HID_DEACTIVATE_DEVICE:
		case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
		default:
			status = STATUS_NOT_SUPPORTED;
			TraceEvents(
				TRACE_LEVEL_WARNING, 
				TRACE_QUEUE, 
				"%!FUNC!: %s is not yet implemented", 
				DbgIoControlGetString(IoControlCode)
			);
			break;
	}

	if (requestPending != TRUE) {
		WdfRequestComplete(
			Request, 
			status
		);
	}

	return;
}

VOID
AmtPtpDeviceEvtIoStop(
	_In_ WDFQUEUE Queue,
	_In_ WDFREQUEST Request,
	_In_ ULONG ActionFlags
)
{

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_QUEUE,
		"%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d",
		Queue, 
		Request, 
		ActionFlags
	);

	//
	// In most cases, the EvtIoStop callback function completes, cancels, or postpones
	// further processing of the I/O request.
	//
	// Typically, the driver uses the following rules:
	//
	// - If the driver owns the I/O request, it either postpones further processing
	//   of the request and calls WdfRequestStopAcknowledge, or it calls WdfRequestComplete
	//   with a completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
	//  
	//   The driver must call WdfRequestComplete only once, to either complete or cancel
	//   the request. To ensure that another thread does not call WdfRequestComplete
	//   for the same request, the EvtIoStop callback must synchronize with the driver's
	//   other event callback functions, for instance by using interlocked operations.
	//
	// - If the driver has forwarded the I/O request to an I/O target, it either calls
	//   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
	//   further processing of the request and calls WdfRequestStopAcknowledge.
	//
	// A driver might choose to take no action in EvtIoStop for requests that are
	// guaranteed to complete in a small amount of time. For example, the driver might
	// take no action for requests that are completed in one of the driver’s request handlers.
	//

	return;

}

NTSTATUS
AmtPtpDispatchReadReportRequests(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN *Pending
)
{

	NTSTATUS status;
	PDEVICE_CONTEXT devContext;

	status = STATUS_SUCCESS;
	devContext = DeviceGetContext(Device);

	status = WdfRequestForwardToIoQueue(
		Request, 
		devContext->InputQueue
	);
	

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestForwardToIoQueue failed with %!STATUS!", 
			status
		);
		return status;
	} else {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, 
			TRACE_DRIVER,
			"%!FUNC! A report has been forwarded to input queue"
		);
	}

	if (NULL != Pending) {
		*Pending = TRUE;
	}

	return status;

}