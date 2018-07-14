#pragma once
// This is the new Windows 10 trace logger provider
#include <TraceLoggingProvider.h>

EXTERN_C_START

//
// Declare TraceLogger Handler
// TraceLogger GUID {871B1E2D-CC5A-4ADE-B74E-6CF1004EF149}
// Do not confuse with WPP tracing
//
TRACELOGGING_DECLARE_PROVIDER(g_hAmtPtpDeviceTraceProvider);

EXTERN_C_END

//
// Defines a set of events to use
//

#define EVENT_DRIVER_FUNCTIONAL		"DriverFunctionalEvent"
#define EVENT_DEVICE_IDENTIFICATION "DeviceIdentificationEvent"
#define EVENT_DEVICE_USBOPERATION	"DeviceUsbOperationEvent"
#define EVENT_INPUT_DIAGNOSTICS		"InputDiagnosticsEvent"

#define EVENT_DRIVER_FUNC_SUBTYPE			"Subtype"
#define EVENT_DRIVER_FUNC_SUBTYPE_CRITFAIL	"CriticalFailure"
#define EVENT_DEVICE_ID_SUBTYPE_NOTFOUND	"DeviceNotFoundInRegistry"
#define EVENT_DEVICE_ID_SUBTYPE_HIDREG_NOTFOUND		"DeviceDescriptorNotFoundInRegistry"
