// Queue.h: This file contains the queue definitions.

EXTERN_C_START

//
// This is the context that can be placed per queue
// and would contain per queue information.
//
typedef struct _QUEUE_CONTEXT {

	ULONG placeholder;

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, QueueGetContext)

_IRQL_requires_(PASSIVE_LEVEL)
PCHAR
DbgIoControlGetString(
	_In_ ULONG IoControlCode
);

NTSTATUS
AmtPtpDeviceQueueInitialize(
	_In_ WDFDEVICE Device
);

NTSTATUS
AmtPtpDispatchReadReportRequests(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN *Pending
);

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL AmtPtpDeviceEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP AmtPtpDeviceEvtIoStop;

EXTERN_C_END
