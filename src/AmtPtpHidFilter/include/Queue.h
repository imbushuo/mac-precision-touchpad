// Queue.h: IO queue bits
#pragma once

EXTERN_C_START

// Initialization
NTSTATUS
PtpFilterIoQueueInitialize(
    _In_ WDFDEVICE Device
);

// Queue context
typedef struct _QUEUE_CONTEXT {
    WDFDEVICE   Device;
    WDFIOTARGET DeviceIoTarget;
} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, PtpFilterQueueGetContext)

// Event handlers
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL FilterEvtIoIntDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP FilterEvtIoStop;

EXTERN_C_END
