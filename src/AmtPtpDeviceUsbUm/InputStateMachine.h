// InputStateMachine.h: Input data post-process routines.

#pragma once

enum TOUCH_SESSION_STATE {
	STATE_NEW_SESSION = 0,
	STATE_CONTINUED_SESSION = 1
};

// This will never be a valid origin ID on Magic Trackpad 2
#define INVALID_ORIGIN_ID						255
#define MT2_MAX_SLOT							16

#define MT2_TIP_SWITCH_THRESHOLD				100
#define MT2_PALM_REJECTION_CONTACT_THRESHOLD	350

typedef struct _SM_RUNTIME_INFORMATION {

	// Assign 16 input slots, as this is the max value that Contact ID can report.
	// And this enables O(1) searching
	PTP_CONTACT_RAW					Contacts[MT2_MAX_SLOT];

	double							LowerBoundary;
	double							UpperBoundary;

	// Some fuzz information will be recorded, 
	// however not used at this moment.

	double							HorizonalFuzz;
	double							VerticalFuzz;
	double							PressureFuzz;
	double							WidthFuzz;
	double							OrientationFuzz;
	enum TOUCH_SESSION_STATE		State;

} SM_RUNTIME_INFORMATION, *PSM_RUNTIME_INFORMATION;

typedef struct _SM_PROJECTED_INFORMATION {

	PTP_CONTACT						Contacts[PTP_MAX_CONTACT_POINTS];
	UCHAR							ReportedContactsCount;

} SM_PROJECTED_INFORMATION, *PSM_PROJECTED_INFORMATION;

_IRQL_requires_(PASSIVE_LEVEL)
HRESULT SmHandleInputRoutine(
	_In_ PTP_CONTACT_RAW pointers[MT2_MAX_SLOT],
	_In_ INT actualPointers,
	_In_ PSM_RUNTIME_INFORMATION pState
);

_IRQL_requires_(PASSIVE_LEVEL)
HRESULT SmResetState(
	_In_ PSM_RUNTIME_INFORMATION pState
);

_IRQL_requires_(PASSIVE_LEVEL)
VOID SmGetPtpReportSlots(
	_In_  PSM_RUNTIME_INFORMATION pState,
	_In_ PSM_PROJECTED_INFORMATION pProjectedInformation
);

_IRQL_requires_(PASSIVE_LEVEL)
static INT SmDefuzzInput(
	_In_ int NewValue,
	_In_ int OldValue,
	_In_ double Fuzz
);
