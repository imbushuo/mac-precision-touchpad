// Driver.h: common definition
#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <usb.h>
#include <usbdlib.h>
#include <wdfusb.h>
#include <initguid.h>
#include <hidport.h>

#include "Trace.h"

EXTERN_C_START

// Common entry points
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD PtpFilterEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP PtpFilterEvtDriverContextCleanup;

// Device specific routines
#include "Device.h"
#include "Queue.h"

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
} HIDCLASS_DRIVER_EXTENSION;

typedef struct _IO_CLIENT_EXTENSION
{
    struct _IO_CLIENT_EXTENSION* NextExtension;
    PVOID	ClientIdentificationAddress;
} IO_CLIENT_EXTENSION, *PIO_CLIENT_EXTENSION, **PPIO_CLIENT_EXTENSION;

typedef struct _DRIVER_EXTENSION_EXT {
    struct _DRIVER_OBJECT* DriverObject;
    PDRIVER_ADD_DEVICE AddDevice;
    ULONG Count;
    UNICODE_STRING ServiceKeyName;
    PIO_CLIENT_EXTENSION IoClientExtension;
} DRIVER_EXTENSION_EXT, *PDRIVER_EXTENSION_EXT;

#define HID_CLASS_EXTENSION_LITERAL_ID "HIDCLASS"
#define HID_USB_SERVICE_NAME L"HidUsb"

// Hack: import
extern NTKERNELAPI
PDEVICE_OBJECT
IoGetLowerDeviceObject(
    _In_  PDEVICE_OBJECT  DeviceObject
);

EXTERN_C_END
