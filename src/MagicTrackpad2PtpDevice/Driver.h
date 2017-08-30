// Driver.h: Driver definitions

#include <windows.h>
#include <wdf.h>
#include <usb.h>
#include <wdfusb.h>
#include <initguid.h>
#include <hidport.h>

#include "Types.h"
#include "AppleDefinition.h"
#include "device.h"
#include "queue.h"
#include "trace.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD MagicTrackpad2PtpDeviceEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP MagicTrackpad2PtpDeviceEvtDriverContextCleanup;

//
// This is the default report descriptor for the Hid device provided
// by the mini driver in response to IOCTL_HID_GET_REPORT_DESCRIPTOR.
// 

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

EXTERN_C_END
