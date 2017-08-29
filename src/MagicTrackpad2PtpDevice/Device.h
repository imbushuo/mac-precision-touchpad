// Device.h: Device definitions

EXTERN_C_START

// Device context struct
typedef struct _DEVICE_CONTEXT
{
	WDFUSBDEVICE						UsbDevice;
	WDFUSBPIPE							InterruptPipe;
	WDFUSBINTERFACE						UsbInterface;

	USB_DEVICE_DESCRIPTOR				DeviceDescriptor;

	const struct BCM5974_CONFIG			*DeviceInfo;

	ULONG								UsbDeviceTraits;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device's queues and callbacks
//
NTSTATUS
MagicTrackpad2PtpDeviceCreateDevice(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
);

//
// Function to select the device's USB configuration and get a WDFUSBDEVICE
// handle
//
EVT_WDF_DEVICE_PREPARE_HARDWARE MagicTrackpad2PtpDeviceEvtDevicePrepareHardware;

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2PtpDeviceConfigContReaderForInterruptEndPoint(
	_In_ PDEVICE_CONTEXT DeviceContext
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
SelectInterruptInterface(
	_In_ WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2PtpDeviceSetPowerPolicy(
	_In_ WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2PtpDeviceSetWellspringMode(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ BOOL IsWellspringModeOn
);

_IRQL_requires_(PASSIVE_LEVEL)
PCHAR
DbgDevicePowerString(
	_In_ WDF_POWER_DEVICE_STATE Type
);

NTSTATUS
MagicTrackpad2PtpDeviceEvtDeviceD0Entry(
	WDFDEVICE Device,
	WDF_POWER_DEVICE_STATE PreviousState
);

NTSTATUS
MagicTrackpad2PtpDeviceEvtDeviceD0Exit(
	WDFDEVICE Device,
	WDF_POWER_DEVICE_STATE TargetState
);

VOID
MagicTrackpad2PtpDeviceEvtUsbInterruptPipeReadComplete(
	WDFUSBPIPE  Pipe,
	WDFMEMORY   Buffer,
	size_t      NumBytesTransferred,
	WDFCONTEXT  Context
);

BOOLEAN
MagicTrackpad2PtpDeviceEvtUsbInterruptReadersFailed(
	_In_ WDFUSBPIPE Pipe,
	_In_ NTSTATUS Status,
	_In_ USBD_STATUS UsbdStatus
);

EXTERN_C_END
