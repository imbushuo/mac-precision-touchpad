// Driver.c: This file contains the driver entry points and callbacks.

#include "driver.h"
#include "driver.tmh"


NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	WDF_DRIVER_CONFIG config;
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES attributes;

	//
	// Initialize WPP Tracing
	//
	WPP_INIT_TRACING(DriverObject, RegistryPath);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Register a cleanup callback so that we can call WPP_CLEANUP when
	// the framework driver object is deleted during driver unload.
	//
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.EvtCleanupCallback = MagicTrackpad2PtpDeviceEvtDriverContextCleanup;

	WDF_DRIVER_CONFIG_INIT(&config,
		MagicTrackpad2PtpDeviceEvtDeviceAdd
	);

	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		&attributes,
		&config,
		WDF_NO_HANDLE
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
		WPP_CLEANUP(DriverObject);
		return status;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;
}

NTSTATUS
MagicTrackpad2PtpDeviceEvtDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{
	NTSTATUS status = STATUS_SUCCESS;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
	status = MagicTrackpad2PtpDeviceCreateDevice(Driver, DeviceInit);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;
}

VOID
MagicTrackpad2PtpDeviceEvtDriverContextCleanup(
	_In_ WDFOBJECT DriverObject
)
{
	// TODO: Perform additional cleanup

	UNREFERENCED_PARAMETER(DriverObject);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Stop WPP Tracing
	//
	WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));
}
