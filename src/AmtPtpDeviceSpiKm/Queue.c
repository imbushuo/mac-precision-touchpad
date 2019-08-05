/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AmtPtpDeviceSpiKmQueueInitialize)
#endif

NTSTATUS
AmtPtpDeviceSpiKmQueueInitialize(
    _In_ WDFDEVICE Device
    )
{
    WDFQUEUE Queue;
    NTSTATUS Status;
    WDF_IO_QUEUE_CONFIG QueueConfig;
	PDEVICE_CONTEXT pDeviceContext;

    PAGED_CODE();

	// By the time this is being called, it should exist
	pDeviceContext = DeviceGetContext(Device);

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
        &QueueConfig,
        WdfIoQueueDispatchParallel
    );

	QueueConfig.EvtIoInternalDeviceControl = AmtPtpDeviceSpiKmEvtIoInternalDeviceControl;
    QueueConfig.EvtIoStop = AmtPtpDeviceSpiKmEvtIoStop;

    Status = WdfIoQueueCreate(
        Device,
        &QueueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &Queue
    );

    if(!NT_SUCCESS(Status)) 
	{
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", Status);
		goto exit;
    }

	//
	// Create secondary queues for touch read requests.
	//
	WDF_IO_QUEUE_CONFIG_INIT(&QueueConfig, WdfIoQueueDispatchManual);
	QueueConfig.PowerManaged = WdfFalse;

	Status = WdfIoQueueCreate(
		Device,
		&QueueConfig,
		WDF_NO_OBJECT_ATTRIBUTES,
		&pDeviceContext->HidQueue
	);

	if (!NT_SUCCESS(Status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_QUEUE, 
			"%!FUNC! WdfIoQueueCreate (Input) failed %!STATUS!",
			Status
		);
	}

exit:
    return Status;
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
	case IOCTL_HID_GET_FEATURE:
		return "IOCTL_HID_GET_FEATURE";
	case IOCTL_HID_SET_FEATURE:
		return "IOCTL_HID_SET_FEATURE";
	default:
		return "IOCTL_UNKNOWN";
	}
}

VOID
AmtPtpDeviceSpiKmEvtIoInternalDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
{
	NTSTATUS Status = STATUS_SUCCESS;
	WDFDEVICE Device = WdfIoQueueGetDevice(Queue);
	BOOLEAN RequestPending = FALSE;
	PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(Device);

    TraceEvents(
		TRACE_LEVEL_INFORMATION, 
        TRACE_QUEUE, 
        "%!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
        Queue, 
		Request, 
		(int) OutputBufferLength, 
		(int) InputBufferLength, 
		IoControlCode
	);

	if (!pDeviceContext->DeviceReady)
	{
		WdfRequestComplete(
			Request,
			STATUS_CANCELLED
		);

		return;
	}

	// Dispatch IOCTL to handler
	switch (IoControlCode)
	{
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		Status = AmtPtpGetHidDescriptor(
			Device,
			Request
		);
		break;
	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		Status = AmtPtpGetDeviceAttribs(
			Device,
			Request
		);
		break;
	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		Status = AmtPtpGetReportDescriptor(
			Device,
			Request
		);
		break;
	case IOCTL_HID_GET_STRING:
		Status = AmtPtpGetStrings(
			Device,
			Request,
			&RequestPending
		);
		break;
	case IOCTL_HID_READ_REPORT:
		AmtPtpSpiInputRoutineWorker(
			Device,
			Request
		);
		RequestPending = TRUE;
		break;
	case IOCTL_HID_GET_FEATURE:
		Status = AmtPtpReportFeatures(
			Device,
			Request
		);
		break;
	case IOCTL_HID_SET_FEATURE:
		Status = AmtPtpSetFeatures(
			Device,
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
		Status = STATUS_NOT_SUPPORTED;
		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_QUEUE,
			"%!FUNC!: %s is not yet implemented",
			DbgIoControlGetString(IoControlCode)
		);
		break;
	}

	if (RequestPending != TRUE) 
	{
		WdfRequestComplete(
			Request,
			Status
		);
	}

    return;
}

VOID
AmtPtpDeviceSpiKmEvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
                Queue, Request, ActionFlags);

    //
    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
    // further processing of the I/O request.
    //
    // Typically, the driver uses the following rules:
    //
    // - If the driver owns the I/O request, it calls WdfRequestUnmarkCancelable
    //   (if the request is cancelable) and either calls WdfRequestStopAcknowledge
    //   with a Requeue value of TRUE, or it calls WdfRequestComplete with a
    //   completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
    //
    //   Before it can call these methods safely, the driver must make sure that
    //   its implementation of EvtIoStop has exclusive access to the request.
    //
    //   In order to do that, the driver must synchronize access to the request
    //   to prevent other threads from manipulating the request concurrently.
    //   The synchronization method you choose will depend on your driver's design.
    //
    //   For example, if the request is held in a shared context, the EvtIoStop callback
    //   might acquire an internal driver lock, take the request from the shared context,
    //   and then release the lock. At this point, the EvtIoStop callback owns the request
    //   and can safely complete or requeue the request.
    //
    // - If the driver has forwarded the I/O request to an I/O target, it either calls
    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
    //   further processing of the request and calls WdfRequestStopAcknowledge with
    //   a Requeue value of FALSE.
    //
    // A driver might choose to take no action in EvtIoStop for requests that are
    // guaranteed to complete in a small amount of time.
    //
    // In this case, the framework waits until the specified request is complete
    // before moving the device (or system) to a lower power state or removing the device.
    // Potentially, this inaction can prevent a system from entering its hibernation state
    // or another low system power state. In extreme cases, it can cause the system
    // to crash with bugcheck code 9F.
    //

    return;
}
