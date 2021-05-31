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
#include "Hac.h"
#include "Diagnostics.h"
#include "Metadata/StaticHidRegistry.h"
#include "HidMiniport.h"
#include "Input.h"

// Pool Tag
#define PTP_LIST_POOL_TAG 'LTPA'
#define REPORT_BUFFER_SIZE   1024

EXTERN_C_END
