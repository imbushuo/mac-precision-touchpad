// Input.h: Input processing and device definitions
#pragma once

VOID
PtpFilterInputProcessRequest(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

VOID
PtpFilterWorkItemCallback(
	_In_ WDFWORKITEM WorkItem
);

VOID
PtpFilterInputIssueTransportRequest(
	_In_ WDFDEVICE Device
);

VOID
PtpFilterInputRequestCompletionCallback(
	_In_ WDFREQUEST Request,
	_In_ WDFIOTARGET Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS Params,
	_In_ WDFCONTEXT Context
);
