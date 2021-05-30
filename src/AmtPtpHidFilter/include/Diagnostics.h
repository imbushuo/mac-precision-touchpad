#pragma once

EXTERN_C_START

VOID PtpFilterDiagnosticsInitializeContinuousRead(
	_In_ WDFDEVICE Device
);

VOID
PtpFilterDiagnosticsRequestCompletionRoutine(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
);

EXTERN_C_END
