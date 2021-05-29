// Device.c: Device-specific D0<->D3 handler and other misc procedures

#include <Driver.h>
#include "Device.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, PtpFilterCreateDevice)
#endif

NTSTATUS
PtpFilterCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
    WDFDEVICE device;
    PDEVICE_CONTEXT deviceContext;
    NTSTATUS status;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");

    // Initialize Power Callback
    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
    pnpPowerCallbacks.EvtDevicePrepareHardware = PtpFilterPrepareHardware;
    pnpPowerCallbacks.EvtDeviceD0Entry = PtpFilterDeviceD0Entry;
    pnpPowerCallbacks.EvtDeviceD0Exit = PtpFilterDeviceD0Exit;
    pnpPowerCallbacks.EvtDeviceSelfManagedIoInit = PtpFilterSelfManagedIoInit;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);

    // Create WDF device object
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreate failed: %!STATUS!", status);
        goto exit;
    }

    // Initialize context and interface
    deviceContext = PtpFilterGetContext(device);
    deviceContext->Device = device;
    deviceContext->HidIoTarget = WdfDeviceGetIoTarget(device);
    deviceContext->WdmDeviceObject = WdfDeviceWdmGetDeviceObject(device);
    if (deviceContext->WdmDeviceObject == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceWdmGetDeviceObject failed");
        goto exit;
    }

    status = WdfDeviceCreateDeviceInterface(device,&GUID_DEVICEINTERFACE_AmtPtpHidFilter, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "WdfDeviceCreateDeviceInterface failed: %!STATUS!", status);
        goto exit;
    }

    // Initialize IO queue
    status = PtpFilterIoQueueInitialize(device);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "PtpFilterIoQueueInitialize failed: %!STATUS!", status);
    }

exit:
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
    return status;
}

NTSTATUS
PtpFilterPrepareHardware(
    _In_ WDFDEVICE Device,
    _In_ WDFCMRESLIST ResourceList,
    _In_ WDFCMRESLIST ResourceListTranslated
)
{
    PDEVICE_CONTEXT deviceContext;
    NTSTATUS status = STATUS_SUCCESS;
    
    // We don't need to retrieve resources since this works as a filter now
    UNREFERENCED_PARAMETER(ResourceList);
    UNREFERENCED_PARAMETER(ResourceListTranslated);

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");
    deviceContext = PtpFilterGetContext(Device);

    // Initialize IDs, set to zero
    deviceContext->VendorID = 0;
    deviceContext->ProductID = 0;
    deviceContext->VersionNumber = 0;

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
    return status;
}

NTSTATUS
PtpFilterDeviceD0Entry(
    _In_ WDFDEVICE Device,
    _In_ WDF_POWER_DEVICE_STATE PreviousState
)
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(PreviousState);
    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
    return status;
}

NTSTATUS
PtpFilterDeviceD0Exit(
    _In_ WDFDEVICE Device,
    _In_ WDF_POWER_DEVICE_STATE TargetState
)
{
    UNREFERENCED_PARAMETER(Device);
    UNREFERENCED_PARAMETER(TargetState);

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");

    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", STATUS_SUCCESS);
    return STATUS_SUCCESS;
}

NTSTATUS
PtpFilterHijackWindowsHIDStack(
    _In_ WDFDEVICE Device
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_CONTEXT deviceContext;
    PDEVICE_OBJECT  hidTransportWdmDeviceObject = NULL;
    PDRIVER_OBJECT  hidTransportWdmDriverObject = NULL;
    PIO_CLIENT_EXTENSION hidTransportIoClientExtension = NULL;
    PHIDCLASS_DRIVER_EXTENSION hidTransportClassExtension = NULL;
    UNICODE_STRING HidUsbServiceNameString;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");
    deviceContext = PtpFilterGetContext(Device);

    if (deviceContext->WdmDeviceObject == NULL || deviceContext->WdmDeviceObject->DriverObject == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WDM Device Object or Driver Object is null, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    // Access the driver object to find next low-level device (in our case, we expect it to be HID transport driver)
    hidTransportWdmDeviceObject = IoGetLowerDeviceObject(deviceContext->WdmDeviceObject);
    if (hidTransportWdmDeviceObject == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! IoGetLowerDeviceObject returns null, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    hidTransportWdmDriverObject = hidTransportWdmDeviceObject->DriverObject;
    if (hidTransportWdmDriverObject == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! DriverObject of HID transport Device Object is null, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    // Verify if the driver extension is what we expected.
    if (hidTransportWdmDriverObject->DriverExtension == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! DriverExtension of HID transport Driver Object is null, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    // Just two more check...
    hidTransportIoClientExtension = ((PDRIVER_EXTENSION_EXT)hidTransportWdmDriverObject->DriverExtension)->IoClientExtension;
    if (hidTransportIoClientExtension == NULL) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! IO Extension is NULL, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    if (strncmp(HID_CLASS_EXTENSION_LITERAL_ID, hidTransportIoClientExtension->ClientIdentificationAddress, sizeof(HID_CLASS_EXTENSION_LITERAL_ID)) != 0) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! IO Extension mistmatch, can't continue");
        status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    hidTransportClassExtension = (PHIDCLASS_DRIVER_EXTENSION)(hidTransportIoClientExtension + 1);
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Sanity check seems okay, safe to patch IO handler routines");

    // HIDClass overrides:
    // IRP_MJ_SYSTEM_CONTROL, IRP_MJ_WRITE, IRP_MJ_READ, IRP_MJ_POWER, IRP_MJ_PNP, IRP_MJ_INTERNAL_DEVICE_CONTROL, IRP_MJ_DEVICE_CONTROL
    // IRP_MJ_CREATE, IRP_MJ_CLOSE
    // For us, overriding IRP_MJ_DEVICE_CONTROL and IRP_MJ_INTERNAL_DEVICE_CONTROL might be sufficient.
    // Details: https://ligstd.visualstudio.com/Apple%20PTP%20Trackpad/_wiki/wikis/Apple-PTP-Trackpad.wiki/47/Hijack-HIDCLASS
    hidTransportWdmDriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = hidTransportClassExtension->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL];
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! IRP_MJ_INTERNAL_DEVICE_CONTROL patched");

cleanup:
    if (hidTransportWdmDeviceObject != NULL) {
        ObDereferenceObject(hidTransportWdmDeviceObject);
    }
exit:
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
    return status;
}

NTSTATUS
PtpFilterSelfManagedIoInit(
    _In_ WDFDEVICE Device
)
{
    NTSTATUS status;
    PDEVICE_CONTEXT deviceContext;
    WDF_MEMORY_DESCRIPTOR hidAttributeMemoryDescriptor;
    HID_DEVICE_ATTRIBUTES deviceAttributes;

    PAGED_CODE();
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");

    deviceContext = PtpFilterGetContext(Device);
    status = PtpFilterHijackWindowsHIDStack(Device);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! PtpFilterHijackWindowsHIDStack failed, Status = %!STATUS!", status);
        goto exit;
    }

    // Request device attribute descriptor for self-identification.
    RtlZeroMemory(&deviceAttributes, sizeof(deviceAttributes));
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
        &hidAttributeMemoryDescriptor,
        (PVOID)&deviceAttributes,
        sizeof(deviceAttributes)
    );

    status = WdfIoTargetSendInternalIoctlSynchronously(
        deviceContext->HidIoTarget, NULL,
        IOCTL_HID_GET_DEVICE_ATTRIBUTES,
        NULL, &hidAttributeMemoryDescriptor, NULL, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_DEVICE, "%!FUNC! WdfIoTargetSendInternalIoctlSynchronously failed, Status = %!STATUS!", status);
        goto exit;
    }

    deviceContext->VendorID = deviceAttributes.VendorID;
    deviceContext->ProductID = deviceAttributes.ProductID;
    deviceContext->VersionNumber = deviceAttributes.VersionNumber;
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Device %x:%x, Version 0x%x", deviceContext->VendorID,
        deviceContext->ProductID, deviceContext->VersionNumber);
    
exit:
    TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
    return status;
}
