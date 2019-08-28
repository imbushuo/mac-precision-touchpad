/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <hidport.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

#include "AppleDefinition.h"
#include "Hid.h"
#include "Input.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD AmtPtpDeviceSpiKmEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP AmtPtpDeviceSpiKmEvtDriverContextCleanup;

//
// Pool Tag
//
#define PTP_LIST_POOL_TAG 'LTPA'

//
// State Switch Max Retries
//
#define STATE_SWITCH_MAX_RETRIES 5

EXTERN_C_END
