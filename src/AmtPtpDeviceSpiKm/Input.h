#pragma once

KSTART_ROUTINE AmtPtpSpiInputThreadRoutine;

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineWorker(
	WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpRequestCompletionRoutine(
	WDFREQUEST Request,
	WDFIOTARGET Target,
	PWDF_REQUEST_COMPLETION_PARAMS Params,
	WDFCONTEXT Context
);
