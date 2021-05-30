// Hac.h: Yes, we have to hack Windows HID stack on the fly...
#pragma once

EXTERN_C_START

// HACK: detour the HIDClass extension
typedef struct _HIDCLASS_DRIVER_EXTENSION {
    PDRIVER_OBJECT      MinidriverObject;
    UNICODE_STRING      RegistryPath;
    ULONG               DeviceExtensionSize;
    PDRIVER_DISPATCH    MajorFunction[IRP_MJ_MAXIMUM_FUNCTION + 1];
    PDRIVER_ADD_DEVICE  AddDevice;
    PDRIVER_UNLOAD      DriverUnload;
    LONG                ReferenceCount;
    LIST_ENTRY          ListEntry;
    BOOLEAN             DevicesArePolled;
} HIDCLASS_DRIVER_EXTENSION, * PHIDCLASS_DRIVER_EXTENSION;

typedef struct _IO_CLIENT_EXTENSION
{
    struct _IO_CLIENT_EXTENSION* NextExtension;
    PVOID	ClientIdentificationAddress;
} IO_CLIENT_EXTENSION, * PIO_CLIENT_EXTENSION, ** PPIO_CLIENT_EXTENSION;

typedef struct _DRIVER_EXTENSION_EXT {
    struct _DRIVER_OBJECT* DriverObject;
    PDRIVER_ADD_DEVICE AddDevice;
    ULONG Count;
    UNICODE_STRING ServiceKeyName;
    PIO_CLIENT_EXTENSION IoClientExtension;
} DRIVER_EXTENSION_EXT, * PDRIVER_EXTENSION_EXT;

#define HID_CLASS_EXTENSION_LITERAL_ID "HIDCLASS"
#define HID_USB_SERVICE_NAME L"HidUsb"

// Hack: import from ntifs.h
extern NTKERNELAPI
PDEVICE_OBJECT
IoGetLowerDeviceObject(
    _In_  PDEVICE_OBJECT  DeviceObject
);

// Detour routines
NTSTATUS
PtpFilterDetourWindowsHIDStack(
    _In_ WDFDEVICE Device
);


EXTERN_C_END
