// Driver.c: This file contains the driver entry points and callbacks.

#include <driver.h>
#include "driver.tmh"

// Declares Windows 10 TraceLogger provider here.
TRACELOGGING_DEFINE_PROVIDER(
	g_hAmtPtpDeviceTraceProvider,
	"AmtPtpDeviceTraceProvider",
	(0x871b1e2d, 0xcc5a, 0x4ade, 0xb7, 0x4e, 0x6c, 0xf1, 0x0, 0x4e, 0xf1, 0x49));

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	WDF_DRIVER_CONFIG config;
	NTSTATUS status;
	WDF_OBJECT_ATTRIBUTES attributes;

	// Initialize tracing
	DriverTraceInit(
		DriverObject,
		RegistryPath
	);
	
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	//
	// Register a cleanup callback so that we can call WPP_CLEANUP when
	// the framework driver object is deleted during driver unload.
	//
	WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
	attributes.EvtCleanupCallback = AmtPtpDeviceEvtDriverContextCleanup;

	WDF_DRIVER_CONFIG_INIT(&config,
		AmtPtpDeviceEvtDeviceAdd
	);

	status = WdfDriverCreate(DriverObject,
		RegistryPath,
		&attributes,
		&config,
		WDF_NO_HANDLE
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfDriverCreate failed %!STATUS!", 
			status
		);
		TraceLoggingWrite(
			g_hAmtPtpDeviceTraceProvider,
			EVENT_DRIVER_FUNCTIONAL,
			TraceLoggingString("DriverEntry", "Routine"),
			TraceLoggingString("WdfDriverCreate", "Context"),
			TraceLoggingInt32(status, "Status"),
			TraceLoggingString(EVENT_DRIVER_FUNC_SUBTYPE_CRITFAIL, EVENT_DRIVER_FUNC_SUBTYPE)
		);
		DriverTraceCleanup(DriverObject);
		return status;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;
}

VOID
DriverTraceInit(
	_In_ PDRIVER_OBJECT  DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	// Initialize WPP Tracing
	WPP_INIT_TRACING(
		DriverObject,
		RegistryPath
	);

	// Initialize TraceLogger Tracing
	TraceLoggingRegister(g_hAmtPtpDeviceTraceProvider);
}

VOID
DriverTraceCleanup(
	_In_ WDFOBJECT DriverObject
)
{
	UNREFERENCED_PARAMETER(DriverObject);

	TraceLoggingUnregister(g_hAmtPtpDeviceTraceProvider);
	// This actually directly calls WppCleanupUm() in UMDF drivers
	WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER) Driver));
}

NTSTATUS
AmtPtpDeviceEvtDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
)
{
	NTSTATUS status = STATUS_SUCCESS;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER, 
		"%!FUNC! Set FDO driver filter"
	);

	WdfFdoInitSetFilter(DeviceInit);

	status = AmtPtpCreateDevice(
		Driver, 
		DeviceInit
	);

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;
}

VOID
AmtPtpDeviceEvtDriverContextCleanup(
	_In_ WDFOBJECT DriverObject
)
{
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	//
	// Stop WPP Tracing
	//
	DriverTraceCleanup(DriverObject);
}
