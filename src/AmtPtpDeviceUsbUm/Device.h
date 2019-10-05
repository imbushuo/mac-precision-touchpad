// Device.h: Device definitions

EXTERN_C_START

// Device context struct
typedef struct _DEVICE_CONTEXT
{
	WDFUSBDEVICE                UsbDevice;
	WDFUSBPIPE                  InterruptPipe;
	WDFUSBINTERFACE             UsbInterface;
	WDFQUEUE                    InputQueue;

	USB_DEVICE_DESCRIPTOR       DeviceDescriptor;

	const struct BCM5974_CONFIG *DeviceInfo;

	ULONG                       UsbDeviceTraits;

	UCHAR						PressureQualLevel;
	UCHAR						SgContactSizeQualLevel;
	UCHAR						MuContactSizeQualLevel;

	BOOL                        IsWellspringModeOn;
	BOOL                        IsSurfaceReportOn;
	BOOL                        IsButtonReportOn;

	LARGE_INTEGER				PerfCounter;

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
AmtPtpCreateDevice(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
);

//
// Function to select the device's USB configuration and get a WDFUSBDEVICE
// handle
//
EVT_WDF_DEVICE_PREPARE_HARDWARE AmtPtpEvtDevicePrepareHardware;
EVT_WDF_DEVICE_D0_ENTRY AmtPtpEvtDeviceD0Entry;
EVT_WDF_DEVICE_D0_EXIT AmtPtpEvtDeviceD0Exit;

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpConfigContReaderForInterruptEndPoint(
	_In_ PDEVICE_CONTEXT DeviceContext
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetWellspringMode(
	_In_  PDEVICE_CONTEXT DeviceContext,
	_Out_ BOOL* IsWellspringModeOn
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
SelectInterruptInterface(
	_In_ WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetWellspringMode(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ BOOL IsWellspringModeOn
);

_IRQL_requires_(PASSIVE_LEVEL)
PCHAR
DbgDevicePowerString(
	_In_ WDF_POWER_DEVICE_STATE Type
);

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpEvtUsbInterruptPipeReadComplete(
	_In_ WDFUSBPIPE  Pipe,
	_In_ WDFMEMORY   Buffer,
	_In_ size_t      NumBytesTransferred,
	_In_ WDFCONTEXT  Context
);

_IRQL_requires_(PASSIVE_LEVEL)
BOOLEAN
AmtPtpEvtUsbInterruptReadersFailed(
	_In_ WDFUSBPIPE Pipe,
	_In_ NTSTATUS Status,
	_In_ USBD_STATUS UsbdStatus
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpServiceTouchInputInterrupt(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpServiceTouchInputInterruptType5(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ UCHAR* Buffer,
	_In_ size_t NumBytesTransferred
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpEmergResetDevice(
	_In_ PDEVICE_CONTEXT DeviceContext
);

///
/// HID sections
///

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

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
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

//
// Utils
//

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
RequestGetHidXferPacketToReadFromDevice(
	_In_  WDFREQUEST        Request,
	_Out_ HID_XFER_PACKET  *Packet
);

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
RequestGetHidXferPacketToWriteToDevice(
	_In_  WDFREQUEST        Request,
	_Out_ HID_XFER_PACKET  *Packet
);

// Helper function for numberic operation
static inline INT AmtRawToInteger(
	_In_ USHORT x
);

EXTERN_C_END
