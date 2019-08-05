/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"

EXTERN_C_START

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
	// USB Things
	WDFUSBDEVICE UsbDevice;
	WDFUSBPIPE InterruptPipe;
	WDFUSBINTERFACE UsbInterface;
	WDFQUEUE InputQueue;
	USB_DEVICE_DESCRIPTOR DeviceDescriptor;
	ULONG UsbDeviceTraits;

	// Device Config
	const struct BCM5974_CONFIG* DeviceInfo;
	BOOLEAN IsWellspringModeOn;

	// PTP Status
	BOOLEAN PtpInputOn;
	BOOLEAN PtpReportTouch;
	BOOLEAN PtpReportButton;

	// Timer
	LARGE_INTEGER LastReportTime;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Pool tags
//
#define POOL_TAG_PTP_CONTROL 'PTPC'

//
// Function to initialize the device's queues and callbacks
//
NTSTATUS
AmtPtpDeviceUsbKmCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

//
// Function to select the device's USB configuration and get a WDFUSBDEVICE
// handle
//
EVT_WDF_DEVICE_PREPARE_HARDWARE AmtPtpDeviceUsbKmEvtDevicePrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY AmtPtpEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT AmtPtpEvtDeviceD0Exit;

//
// Function to select interrupt interface
//
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
SelectInterruptInterface(
	_In_ WDFDEVICE Device
);

//
// Function to configure interrupt
//
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpConfigContReaderForInterruptEndPoint(
	_In_ PDEVICE_CONTEXT DeviceContext
);

//
// Functions to serve interrupt
//
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
AmtPtpEvtUsbInterruptPipeReadComplete(
	_In_ WDFUSBPIPE  Pipe,
	_In_ WDFMEMORY   Buffer,
	_In_ size_t      NumBytesTransferred,
	_In_ WDFCONTEXT  Context
);

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
AmtPtpEvtUsbInterruptReadersFailed(
	_In_ WDFUSBPIPE Pipe,
	_In_ NTSTATUS Status,
	_In_ USBD_STATUS UsbdStatus
);

//
// Debug utilities
//
PCHAR
DbgDevicePowerString(
	_In_ WDF_POWER_DEVICE_STATE Type
);

PCHAR
DbgIoControlGetString(
	_In_ ULONG IoControlCode
);

//
// Device functions
//
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetWellspringMode(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ BOOLEAN IsWellspringModeOn
);

//
// HID routines
//
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

NTSTATUS
AmtPtpDispatchReadReportRequests(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN* Pending
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpReportFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

EXTERN_C_END
