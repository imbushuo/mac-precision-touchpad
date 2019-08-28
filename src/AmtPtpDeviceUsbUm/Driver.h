// Driver.h: Driver definitions

#include <windows.h>
#include <wdf.h>
#include <usb.h>
#include <wdfusb.h>
#include <initguid.h>
#include <hidport.h>

// ModernTrace is for runtime debugging
// Trace is WPP-based, development debugging
#include "ModernTrace.h"
#include "Trace.h"

#include "AppleDefinition.h"
#include "Hid.h"
#include "Device.h"
#include "Queue.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD AmtPtpDeviceEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP AmtPtpDeviceEvtDriverContextCleanup;


//
// Driver initialization routines
//

VOID
DriverTraceInit(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

VOID 
DriverTraceCleanup(
	_In_ WDFOBJECT DriverObject
);

EXTERN_C_END
