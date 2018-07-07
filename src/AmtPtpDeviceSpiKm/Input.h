#pragma once

KSTART_ROUTINE AmtPtpSpiInputThreadRoutine;

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineWorker(
	WDFDEVICE Device
);

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineInternal(
	WDFDEVICE Device,
	WDFREQUEST Request
);
