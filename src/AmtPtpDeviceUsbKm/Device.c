/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.
    
Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, AmtPtpDeviceUsbKmCreateDevice)
#pragma alloc_text (PAGE, AmtPtpDeviceUsbKmEvtDevicePrepareHardware)
#pragma alloc_text (PAGE, AmtPtpEvtDeviceD0Entry)
#pragma alloc_text (PAGE, AmtPtpEvtDeviceD0Exit)
#endif

_IRQL_requires_(PASSIVE_LEVEL)
static const struct BCM5974_CONFIG*
AmtPtpGetDeviceConfig(
	_In_ USB_DEVICE_DESCRIPTOR deviceInfo
)
{
	USHORT id = deviceInfo.idProduct;
	const struct BCM5974_CONFIG* cfg;

	for (cfg = Bcm5974ConfigTable; cfg->ansi; ++cfg) {
		if (cfg->ansi == id || cfg->iso == id || cfg->jis == id) {
			return cfg;
		}
	}

	return NULL;
}

NTSTATUS
AmtPtpDeviceUsbKmCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
    WDF_OBJECT_ATTRIBUTES   deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;

    PAGED_CODE();

	// Initialize power callback (prep, D0 entry & exit)
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = AmtPtpDeviceUsbKmEvtDevicePrepareHardware;
	pnpPowerCallbacks.EvtDeviceD0Entry = AmtPtpEvtDeviceD0Entry;
	pnpPowerCallbacks.EvtDeviceD0Exit = AmtPtpEvtDeviceD0Exit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //
        deviceContext = DeviceGetContext(device);

        //
        // Initialize the context.
        //

        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_AmtPtpDeviceUsbKm,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = AmtPtpDeviceUsbKmQueueInitialize(device);
        }
    }

    return status;
}

NTSTATUS
AmtPtpDeviceUsbKmEvtDevicePrepareHardware(
    _In_ WDFDEVICE Device,
    _In_ WDFCMRESLIST ResourceList,
    _In_ WDFCMRESLIST ResourceListTranslated
    )
/*++

Routine Description:

    In this callback, the driver does whatever is necessary to make the
    hardware ready to use.  In the case of a USB device, this involves
    reading and selecting descriptors.

Arguments:

    Device - handle to a device

Return Value:

    NT status value

--*/
{
    NTSTATUS status;
    PDEVICE_CONTEXT pDeviceContext;
	WDF_USB_DEVICE_INFORMATION deviceInfo;
	ULONG waitWakeEnable = FALSE;

    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    PAGED_CODE();

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = STATUS_SUCCESS;
    pDeviceContext = DeviceGetContext(Device);

    //
    // Create a USB device handle so that we can communicate with the
    // underlying USB stack. The WDFUSBDEVICE handle is used to query,
    // configure, and manage all aspects of the USB device.
    // These aspects include device properties, bus properties,
    // and I/O creation and synchronization. We only create the device the first time
    // PrepareHardware is called. If the device is restarted by pnp manager
    // for resource rebalance, we will use the same device handle but then select
    // the interfaces again because the USB stack could reconfigure the device on
    // restart.
    //
    if (pDeviceContext->UsbDevice == NULL) {
		status = WdfUsbTargetDeviceCreate(Device,
			WDF_NO_OBJECT_ATTRIBUTES,
			&pDeviceContext->UsbDevice
		);

        if (!NT_SUCCESS(status)) {
            TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
				"WdfUsbTargetDeviceCreate failed 0x%x", status);
            return status;
        }
    }

	// Retrieve device information
	WdfUsbTargetDeviceGetDeviceDescriptor(
		pDeviceContext->UsbDevice,
		&pDeviceContext->DeviceDescriptor
	);

	// Get correct configuration from conf store
	pDeviceContext->DeviceInfo = AmtPtpGetDeviceConfig(pDeviceContext->DeviceDescriptor);
	if (pDeviceContext->DeviceInfo == NULL) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
			"AmtPtpGetDeviceConfig failed to find the device config");
		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	//
	// Retrieve USBD version information, port driver capabilites and device
	// capabilites such as speed, power, etc.
	//
	WDF_USB_DEVICE_INFORMATION_INIT(&deviceInfo);
	status = WdfUsbTargetDeviceRetrieveInformation(
		pDeviceContext->UsbDevice,
		&deviceInfo
	);

	if (NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DEVICE,
			"%!FUNC! IsDeviceHighSpeed: %s",
			(deviceInfo.Traits & WDF_USB_DEVICE_TRAIT_AT_HIGH_SPEED) ? "TRUE" : "FALSE");

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DEVICE,
			"%!FUNC! IsDeviceSelfPowered: %s",
			(deviceInfo.Traits & WDF_USB_DEVICE_TRAIT_SELF_POWERED) ? "TRUE" : "FALSE");

		waitWakeEnable = deviceInfo.Traits &
			WDF_USB_DEVICE_TRAIT_REMOTE_WAKE_CAPABLE;

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DEVICE,
			"%!FUNC! IsDeviceRemoteWakeable: %s",
			waitWakeEnable ? "TRUE" : "FALSE");

		//
		// Save these for use later.
		//
		pDeviceContext->UsbDeviceTraits = deviceInfo.Traits;
	}
	else {
		pDeviceContext->UsbDeviceTraits = 0;
	}

	// Select interface to use
	status = SelectInterruptInterface(Device);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, 
			"%!FUNC! SelectInterruptInterface failed with %!STATUS!", status);
		return status;
	}

	// Set up interrupt
	status = AmtPtpConfigContReaderForInterruptEndPoint(pDeviceContext);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, 
			"%!FUNC! AmtPtpConfigContReaderForInterruptEndPoint failed with %!STATUS!", status);
		return status;
	}

	// Set default settings
	pDeviceContext->PtpReportButton = TRUE;
	pDeviceContext->PtpReportTouch = TRUE;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

    return status;
}

// D0 Entry & Exit
NTSTATUS
AmtPtpEvtDeviceD0Entry(
	_In_ WDFDEVICE Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
)
{
	PDEVICE_CONTEXT         pDeviceContext;
	NTSTATUS                status;
	BOOLEAN                 isTargetStarted;

	pDeviceContext = DeviceGetContext(Device);
	isTargetStarted = FALSE;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! -->AmtPtpDeviceEvtDeviceD0Entry - coming from %s",
		DbgDevicePowerString(PreviousState)
	);

	// Check wellspring mode
	if (pDeviceContext->PtpReportButton || pDeviceContext->IsWellspringModeOn) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! <--AmtPtpDeviceEvtDeviceD0Entry - Start Wellspring Mode"
		);

		status = AmtPtpSetWellspringMode(
			pDeviceContext,
			TRUE
		);

		if (!NT_SUCCESS(status)) {
			TraceEvents(
				TRACE_LEVEL_WARNING,
				TRACE_DRIVER,
				"%!FUNC! <--AmtPtpDeviceEvtDeviceD0Entry - Start Wellspring Mode failed with %!STATUS!",
				status
			);
		}
	}

	// Get current time counter
	KeQueryPerformanceCounter(&pDeviceContext->LastReportTime);

	//
	// Since continuous reader is configured for this interrupt-pipe, we must explicitly start
	// the I/O target to get the framework to post read requests.
	//
	status = WdfIoTargetStart(WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe));
	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! <--AmtPtpDeviceEvtDeviceD0Entry - Failed to start interrupt pipe %!STATUS!",
			status
		);
		goto end;
	}

	isTargetStarted = TRUE;

end:
	if (!NT_SUCCESS(status)) {
		//
		// Failure in D0Entry will lead to device being removed. So let us stop the continuous
		// reader in preparation for the ensuing remove.
		//
		if (isTargetStarted) {
			WdfIoTargetStop(
				WdfUsbTargetPipeGetIoTarget(pDeviceContext->InterruptPipe),
				WdfIoTargetCancelSentIo
			);
		}
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! <--AmtPtpDeviceEvtDeviceD0Entry"
	);

	return status;
}

NTSTATUS
AmtPtpEvtDeviceD0Exit(
	_In_ WDFDEVICE Device,
	_In_ WDF_POWER_DEVICE_STATE TargetState
)
{
	PDEVICE_CONTEXT         pDeviceContext;
	NTSTATUS				status;

	PAGED_CODE();
	status = STATUS_SUCCESS;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! -->AmtPtpDeviceEvtDeviceD0Exit - moving to %s",
		DbgDevicePowerString(TargetState)
	);

	pDeviceContext = DeviceGetContext(Device);

	// Stop IO Pipe.
	WdfIoTargetStop(WdfUsbTargetPipeGetIoTarget(
		pDeviceContext->InterruptPipe),
		WdfIoTargetCancelSentIo
	);

	// Cancel Wellspring mode.
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! -->AmtPtpDeviceEvtDeviceD0Exit - Cancel Wellspring Mode"
	);

	status = AmtPtpSetWellspringMode(
		pDeviceContext,
		FALSE
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_DRIVER,
			"%!FUNC! -->AmtPtpDeviceEvtDeviceD0Exit - Cancel Wellspring Mode failed with %!STATUS!",
			status
		);
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! <--AmtPtpDeviceEvtDeviceD0Exit"
	);

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
SelectInterruptInterface(
	_In_ WDFDEVICE Device
)
{
	WDF_USB_DEVICE_SELECT_CONFIG_PARAMS configParams;
	NTSTATUS                            status = STATUS_SUCCESS;
	PDEVICE_CONTEXT                     pDeviceContext;
	WDFUSBPIPE                          pipe;
	WDF_USB_PIPE_INFORMATION            pipeInfo;
	UCHAR                               index;
	UCHAR                               numberConfiguredPipes;

	PAGED_CODE();

	pDeviceContext = DeviceGetContext(Device);
	WDF_USB_DEVICE_SELECT_CONFIG_PARAMS_INIT_SINGLE_INTERFACE(&configParams);

	// It is slightly different than UMDF
	status = WdfUsbTargetDeviceSelectConfig(pDeviceContext->UsbDevice,
		WDF_NO_OBJECT_ATTRIBUTES,
		&configParams);

	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE,
			"WdfUsbTargetDeviceSelectConfig failed %!STATUS! ",
			status);
		return status;
	}

	pDeviceContext->UsbInterface = configParams.Types.SingleInterface.ConfiguredUsbInterface;
	numberConfiguredPipes = configParams.Types.SingleInterface.NumberConfiguredPipes;

	//
	// Get pipe handles
	//
	for (index = 0; index < numberConfiguredPipes; index++) {

		WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);

		pipe = WdfUsbInterfaceGetConfiguredPipe(
			pDeviceContext->UsbInterface,
			index, //PipeIndex,
			&pipeInfo
		);

		//
		// Tell the framework that it's okay to read less than
		// MaximumPacketSize
		//
		WdfUsbTargetPipeSetNoMaximumPacketSizeCheck(pipe);

		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DEVICE,
			"%!FUNC! Found USB pipe type %d",
			pipeInfo.PipeType
		);

		if (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType) {
			pDeviceContext->InterruptPipe = pipe;
			break;
		}

	}

	//
	// If we didn't find interrupt pipe, fail the start.
	//
	if (!pDeviceContext->InterruptPipe) {
		status = STATUS_INVALID_DEVICE_STATE;
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DEVICE,
			"%!FUNC! Device is not configured properly %!STATUS!",
			status
		);

		return status;
	}

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetWellspringMode(
	_In_ PDEVICE_CONTEXT DeviceContext,
	_In_ BOOLEAN IsWellspringModeOn
)
{
	NTSTATUS						status;
	WDF_USB_CONTROL_SETUP_PACKET	setupPacket;
	WDF_MEMORY_DESCRIPTOR			memoryDescriptor;
	ULONG							cbTransferred;
	WDFMEMORY						bufHandle = NULL;
	unsigned char* buffer;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	// Type 3 does not need a mode switch.
	// However, turn mode on or off as requested.
	if (DeviceContext->DeviceInfo->tp_type == TYPE3) {
		DeviceContext->IsWellspringModeOn = IsWellspringModeOn;
		return STATUS_SUCCESS;
	}

	status = WdfMemoryCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		PagedPool,
		0,
		DeviceContext->DeviceInfo->um_size,
		&bufHandle,
		&buffer
	);

	if (!NT_SUCCESS(status)) {
		goto cleanup;
	}

	RtlZeroMemory(
		buffer,
		DeviceContext->DeviceInfo->um_size
	);

	WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
		&memoryDescriptor,
		buffer,
		sizeof(DeviceContext->DeviceInfo->um_size)
	);

	WDF_USB_CONTROL_SETUP_PACKET_INIT(
		&setupPacket,
		BmRequestDeviceToHost,
		BmRequestToInterface,
		BCM5974_WELLSPRING_MODE_READ_REQUEST_ID,
		(USHORT) DeviceContext->DeviceInfo->um_req_val,
		(USHORT) DeviceContext->DeviceInfo->um_req_idx
	);

	// Set stuffs right
	setupPacket.Packet.bm.Request.Type = BmRequestClass;

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
		DeviceContext->UsbDevice,
		WDF_NO_HANDLE,
		NULL,
		&setupPacket,
		&memoryDescriptor,
		&cbTransferred
	);

	// Behavior mismatch: Actual device does not transfer bytes as expected (in length)
	// So we do not check um_size as a temporary workaround.
	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DEVICE,
			"%!FUNC! WdfUsbTargetDeviceSendControlTransferSynchronously (Read) failed with %!STATUS!, cbTransferred = %llu, um_size = %d",
			status,
			cbTransferred,
			DeviceContext->DeviceInfo->um_size
		);
		goto cleanup;
	}

	// Apply the mode switch
	buffer[DeviceContext->DeviceInfo->um_switch_idx] = IsWellspringModeOn ?
		(unsigned char)DeviceContext->DeviceInfo->um_switch_on :
		(unsigned char)DeviceContext->DeviceInfo->um_switch_off;

	// Write configuration
	WDF_USB_CONTROL_SETUP_PACKET_INIT(
		&setupPacket,
		BmRequestHostToDevice,
		BmRequestToInterface,
		BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID,
		(USHORT)DeviceContext->DeviceInfo->um_req_val,
		(USHORT)DeviceContext->DeviceInfo->um_req_idx
	);

	// Set stuffs right
	setupPacket.Packet.bm.Request.Type = BmRequestClass;

	status = WdfUsbTargetDeviceSendControlTransferSynchronously(
		DeviceContext->UsbDevice,
		WDF_NO_HANDLE,
		NULL,
		&setupPacket,
		&memoryDescriptor,
		&cbTransferred
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DEVICE,
			"%!FUNC! WdfUsbTargetDeviceSendControlTransferSynchronously (Write) failed with %!STATUS!",
			status
		);
		goto cleanup;
	}

	// Set status
	DeviceContext->IsWellspringModeOn = IsWellspringModeOn;

cleanup:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	bufHandle = NULL;
	return status;
}
