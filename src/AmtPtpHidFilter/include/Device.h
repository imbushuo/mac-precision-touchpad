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

// Device Context
typedef struct _DEVICE_CONTEXT
{
    PDEVICE_OBJECT  WdmDeviceObject;
    WDFDEVICE       Device;
    WDFQUEUE        SystemIoQueue;

    // Device identification
    USHORT VendorID;
    USHORT ProductID;
    USHORT VersionNumber;

    // System HID transport PDO IO Target
    WDFIOTARGET HidIoTarget;
    BOOLEAN     IsHidIoDetourCompleted;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, PtpFilterGetContext)

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

EXTERN_C_END
