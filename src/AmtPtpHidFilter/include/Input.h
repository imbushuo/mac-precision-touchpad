// Input.h: Input processing and device definitions
#pragma once

VOID
PtpFilterInputProcessRequest(
	WDFDEVICE Device,
	WDFREQUEST Request
);

VOID
PtpFilterInputIssueTransportRequest(
	WDFDEVICE Device
);

VOID
PtpFilterInputRequestCompletionCallback(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
);
