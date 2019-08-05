/*++

Module Name:

    queue.h

Abstract:

    This file contains the queue definitions.

Environment:

    Kernel-mode Driver Framework

--*/

EXTERN_C_START

//
// This is the context that can be placed per queue
// and would contain per queue information.
//
typedef struct _QUEUE_CONTEXT {

    ULONG PrivateDeviceData;  // just a placeholder

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, QueueGetContext)

NTSTATUS
AmtPtpDeviceSpiKmQueueInitialize(
    _In_ WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
PCHAR
DbgIoControlGetString(
	_In_ ULONG IoControlCode
);

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL AmtPtpDeviceSpiKmEvtIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP AmtPtpDeviceSpiKmEvtIoStop;

EXTERN_C_END
