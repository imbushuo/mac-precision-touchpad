// Device.h: Device-specific struct and routines

#pragma once

EXTERN_C_START

// {FF969022-3111-4441-8F88-875440172C2E} for the device interface
DEFINE_GUID(GUID_DEVICEINTERFACE_AmtPtpHidFilter,
    0xff969022, 0x3111, 0x4441, 0x8f, 0x88, 0x87, 0x54, 0x40, 0x17, 0x2c, 0x2e);

// Definition
#define HID_XFER_PACKET_SIZE 32

#define HID_VID_APPLE_USB 0x05ac
#define HID_VID_APPLE_BT  0x004c
#define HID_PID_MAGIC_TRACKPAD_2 0x0265

/* device-specific parameters */
typedef struct _BCM5974_PARAM {
    int snratio;		/* signal-to-noise ratio */
    int min;			/* device minimum reading */
    int max;			/* device maximum reading */
} BCM5974_PARAM, *PBCM5974_PARAM;

// Device Context
typedef struct _DEVICE_CONTEXT
{
    PDEVICE_OBJECT  WdmDeviceObject;
    WDFDEVICE       Device;
    WDFQUEUE        HidReadQueue;

    // Device identification
    USHORT VendorID;
    USHORT ProductID;
    USHORT VersionNumber;
    size_t InputHeaderSize;
    size_t InputFingerSize;
    size_t InputFingerDelta;
    size_t InputButtonDelta;
    BCM5974_PARAM X;
    BCM5974_PARAM Y;

    // List of buffers
    WDFLOOKASIDE HidReadBufferLookaside;

    // System HID transport
    WDFIOTARGET HidIoTarget;
    BOOLEAN     IsHidIoDetourCompleted;
    WDFTIMER    HidTransportRecoveryTimer;

    // Device State
    BOOLEAN DeviceConfigured;

    // PTP report specific
    LARGE_INTEGER   LastReportTime;
    BOOLEAN         PtpInputOn;
    BOOLEAN         PtpReportTouch;
    BOOLEAN         PtpReportButton;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, PtpFilterGetContext)

// Request context
typedef struct _WORKER_REQUEST_CONTEXT {
    PDEVICE_CONTEXT DeviceContext;
    WDFMEMORY RequestMemory;
} WORKER_REQUEST_CONTEXT, * PWORKER_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKER_REQUEST_CONTEXT, WorkerRequestGetContext)

// Initialization routines
NTSTATUS
PtpFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);

// Power Management Handlers
EVT_WDF_DEVICE_PREPARE_HARDWARE PtpFilterPrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY PtpFilterDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT PtpFilterDeviceD0Exit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_INIT PtpFilterSelfManagedIoInit;
EVT_WDF_DEVICE_SELF_MANAGED_IO_RESTART PtpFilterSelfManagedIoRestart;

// Device Management routines
NTSTATUS
PtpFilterConfigureMultiTouch(
    _In_ WDFDEVICE Device
);

VOID
PtpFilterRecoveryTimerCallback(
    WDFTIMER Timer
);

EXTERN_C_END
