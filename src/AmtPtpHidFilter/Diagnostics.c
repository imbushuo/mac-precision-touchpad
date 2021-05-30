// Diagnostics.c: Debug and diagnostics facilities

#include <Driver.h>
#include "Diagnostics.tmh"

VOID PtpFilterDiagnosticsInitializeContinuousRead(
	_In_ WDFDEVICE Device
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT deviceContext;
	WDFREQUEST request;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);
	
	// Issue a request to underlying HID target. Then start to have a continus stream reading it

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DEVICE, "%!FUNC! Exit, Status = %!STATUS!", status);
}

VOID
PtpFilterDiagnosticsRequestCompletionRoutine(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
)
{
	// Decode request

	// Issue next request
}
