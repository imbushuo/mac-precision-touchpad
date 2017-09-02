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

NTSTATUS
MagicTrackpad2PtpDeviceQueueInitialize(
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
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL MagicTrackpad2PtpDeviceEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP MagicTrackpad2PtpDeviceEvtIoStop;

EXTERN_C_END
