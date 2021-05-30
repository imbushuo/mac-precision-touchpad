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

typedef struct _SPI_TRACKPAD_INFO {
	USHORT VendorId;
	USHORT ProductId;
	SHORT XMin;
	SHORT XMax;
	SHORT YMin;
	SHORT YMax;
} SPI_TRACKPAD_INFO, *PSPI_TRACKPAD_INFO;

// Stupid Microsoft only assigns a UCHAR for touch ID
// we could have a better approach
typedef struct _PTP_AAPL_MAPPING {
	SHORT OriginalX;
	SHORT OriginalY;
	INT8 ContactID;
} PTP_AAPL_MAPPING, *PPTP_AAPL_MAPPING;

#define MAPPING_MAX 10

typedef enum _REPORT_TYPE {
	PrecisionTouchpad = 0,
	Touchscreen = 1,
	InvalidReportType = 0x7fffffff,
} REPORT_TYPE;

typedef enum _PTP_AAPL_DEVICE_POWER_STATUS {
	D3 = 0,
	D0ActiveAndConfigured = 1,
	D0ActiveAndUnconfigured = 2
} PTP_AAPL_DEVICE_POWER_STATUS;

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
	// IO content
	WDFDEVICE	SpiDevice;
	WDFIOTARGET SpiTrackpadIoTarget;
	PTP_AAPL_DEVICE_POWER_STATUS DeviceStatus;
	HANDLE		InputPollThreadHandle;
	WDFQUEUE	HidQueue;

	// SPI device metadata
	USHORT HidVendorID;
	USHORT HidProductID;
	USHORT HidVersionNumber;
	SPI_TRACKPAD_INFO TrackpadInfo;
	REPORT_TYPE ReportType;

	// Windows PTP context
	BOOLEAN PtpInputOn;
	BOOLEAN PtpReportTouch;
	BOOLEAN PtpReportButton;

	// Timer
	LARGE_INTEGER LastReportTime;
	WDFTIMER PowerOnRecoveryTimer;

	// List of buffers
	WDFLOOKASIDE HidReadBufferLookaside;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Request context
//
typedef struct _WORKER_REQUEST_CONTEXT {
	PDEVICE_CONTEXT DeviceContext;
	WDFMEMORY RequestMemory;
} WORKER_REQUEST_CONTEXT, *PWORKER_REQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKER_REQUEST_CONTEXT, WorkerRequestGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
AmtPtpDeviceSpiKmCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EVT_WDF_DEVICE_PREPARE_HARDWARE AmtPtpEvtDevicePrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY AmtPtpEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT AmtPtpEvtDeviceD0Exit;

NTSTATUS
AmtPtpEvtDeviceSelfManagedIoInitOrRestart(
	_In_ WDFDEVICE Device
);

PCHAR
DbgDevicePowerString(
	_In_ WDF_POWER_DEVICE_STATE Type
);

NTSTATUS
AmtPtpSpiSetState(
	_In_ WDFDEVICE Device,
	_In_ BOOLEAN DesiredState
);

void AmtPtpPowerRecoveryTimerCallback(
	WDFTIMER Timer
);

EXTERN_C_END
