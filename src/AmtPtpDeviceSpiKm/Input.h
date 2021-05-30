#pragma once

EVT_WDF_REQUEST_COMPLETION_ROUTINE AmtPtpRequestCompletionRoutine;

VOID
AmtPtpSpiInputRoutineWorker(
	WDFDEVICE Device,
	WDFREQUEST PtpRequest
);

VOID
AmtPtpSpiInputIssueRequest(
	WDFDEVICE Device
);
