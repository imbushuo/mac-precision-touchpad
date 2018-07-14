#include "Driver.h"
#include "InputStateMachine.tmh"

_IRQL_requires_(PASSIVE_LEVEL)
HRESULT SmHandleInputRoutine(
	_In_ PTP_CONTACT_RAW pointers[MT2_MAX_SLOT],
	_In_ INT actualPointers,
	_In_ PSM_RUNTIME_INFORMATION pState) 
{

	HRESULT				status			= S_OK;
	BOOLEAN				found			= FALSE;

	int preProcessCounter = 0;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	// Check pointer
	if (pState == NULL) {

		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_INPUT,
			"%!FUNC! Invalid pointer address (argument)"
		);

		status = STATUS_INVALID_PARAMETER;
		goto exit;

	}

	// Verify if contact points are found in registry.
	for (int i = 0; i < actualPointers; i++)
	{
		switch (pState->Contacts[pointers[i].ContactId].State)
		{
		case CONTACT_NEW:
		case CONTACT_CONTINUED:
		case CONTACT_CONFIDENCE_CANCELLED:
			// We don't need to continue searching. Set pivot to null
			found = TRUE;
			break;
		case CONTACT_INVALID:
			break;
		}

		// Exit if flag is set
		if (found) break;
	}

	// If all contact points are not found in registry, reset state machine.
	if (!found) {
		status = SmResetState(pState);
		if (!NT_SUCCESS(status)) goto exit;
		pState->State = STATE_NEW_SESSION;
	}
	else {
		pState->State = STATE_CONTINUED_SESSION;
	}

	// Set information to registry.
	for (int i = 0; i < actualPointers; i++)
	{
		
		UCHAR contactId = pointers[i].ContactId;

		switch (pState->Contacts[contactId].State)
		{
		case CONTACT_NEW:
		case CONTACT_CONTINUED:
			// Input defuzz
			pState->Contacts[contactId].X = (USHORT) SmDefuzzInput(
				pointers[i].X,
				pState->Contacts[contactId].X,
				pState->HorizonalFuzz
			);
			pState->Contacts[contactId].Y = (USHORT) SmDefuzzInput(
				pointers[i].Y,
				pState->Contacts[contactId].Y,
				pState->VerticalFuzz
			);
			// Non-fuzzing values
			pState->Contacts[contactId].TouchMajor = pointers[i].TouchMajor;
			pState->Contacts[contactId].TouchMinor = pointers[i].TouchMinor;
			// Check touch major size
			pState->Contacts[contactId].State = 
				(pointers[i].TouchMajor >= MT2_PALM_REJECTION_CONTACT_THRESHOLD) ?
				CONTACT_CONFIDENCE_CANCELLED : CONTACT_CONTINUED;
			break;
		case CONTACT_INVALID:
			pState->Contacts[contactId].State = CONTACT_NEW;
			pState->Contacts[contactId].ContactId = contactId;
			pState->Contacts[contactId].X = pointers[i].X;
			pState->Contacts[contactId].Y = pointers[i].Y;
			pState->Contacts[contactId].TouchMajor = pointers[i].TouchMajor;
			pState->Contacts[contactId].TouchMinor = pointers[i].TouchMinor;
			// Assign content
			break;
		case CONTACT_CONFIDENCE_CANCELLED:
			// State doesn't change. Defuzz input, still have confidence cancelled.
			// Input defuzz
			pState->Contacts[contactId].X = (USHORT) SmDefuzzInput(
				pointers[i].X,
				pState->Contacts[contactId].X,
				pState->HorizonalFuzz
			);
			pState->Contacts[contactId].Y = (USHORT) SmDefuzzInput(
				pointers[i].Y,
				pState->Contacts[contactId].Y,
				pState->VerticalFuzz
			);
			// Non-fuzzing values
			pState->Contacts[contactId].TouchMajor = pointers[i].TouchMajor;
			pState->Contacts[contactId].TouchMinor = pointers[i].TouchMinor;
			break;
		}

		preProcessCounter++;

	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_INPUT,
		"%!FUNC! Pre processed %d contact(s).",
		preProcessCounter
	);

exit:

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
VOID SmGetPtpReportSlots(
	_In_ PSM_RUNTIME_INFORMATION pState,
	_In_ PSM_PROJECTED_INFORMATION pProjectedInformation
)
{

	UCHAR contactCount = 0;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	if (pState == NULL) {

		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_INPUT,
			"%!FUNC! Invalid pointer address (argument)"
		);

		goto exit;

	}

	for (UCHAR i = 0; i < MT2_MAX_SLOT; i++) {

		if (pState->Contacts[i].State != CONTACT_INVALID) {
			
			pProjectedInformation->Contacts[contactCount].ContactID = i;
			pProjectedInformation->Contacts[contactCount].X = pState->Contacts[i].X;
			pProjectedInformation->Contacts[contactCount].Y = pState->Contacts[i].Y;
			pProjectedInformation->Contacts[contactCount].Confidence = (pState->Contacts[i].State == CONTACT_CONFIDENCE_CANCELLED) ? 0 : 1;
			pProjectedInformation->Contacts[contactCount].TipSwitch = pState->Contacts[i].TouchMajor >= MT2_TIP_SWITCH_THRESHOLD;
			
			contactCount++;

		}

		if (contactCount == 5) {
			// We have 5 contacts to report. Stop. Discard rest
			break;
		}

	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_INPUT,
		"%!FUNC! Post processed %d points.",
		contactCount
	);

	pProjectedInformation->ReportedContactsCount = contactCount;

exit:

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

}

_IRQL_requires_(PASSIVE_LEVEL)
HRESULT SmResetState(
	_In_ PSM_RUNTIME_INFORMATION pState
)
{

	HRESULT status = S_OK;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	if (pState == NULL) {
		// Invalid parameter
		status = STATUS_INVALID_PARAMETER;
		goto exit;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_INPUT,
		"%!FUNC! Resetting input session"
	);

	// Reset state
	pState->State = STATE_NEW_SESSION;

	// Reset all points & contact points
	for (int i = 0; i < MT2_MAX_SLOT; i++) {

		pState->Contacts[i].X = 0;
		pState->Contacts[i].Y = 0;
		pState->Contacts[i].Size = 0;
		pState->Contacts[i].Pressure = 0;
		pState->Contacts[i].TouchMajor = 0;
		pState->Contacts[i].TouchMinor = 0;
		pState->Contacts[i].Orientation = 0;
		pState->Contacts[i].State = CONTACT_INVALID;
		pState->Contacts[i].ContactId = INVALID_ORIGIN_ID;

	}

exit:

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;

}

// Debuzz function from Linux Kernel: drivers/input/input.c
_IRQL_requires_(PASSIVE_LEVEL)
static INT SmDefuzzInput(
	_In_ int NewValue,
	_In_ int OldValue,
	_In_ double Fuzz
)
{

	if (Fuzz) {
		if (NewValue > OldValue - Fuzz / 2 && NewValue < OldValue + Fuzz / 2)
			return OldValue;

		if (NewValue > OldValue - Fuzz && NewValue < OldValue + Fuzz)
			return (OldValue * 3 + NewValue) / 4;

		if (NewValue > OldValue - Fuzz * 2 && NewValue < OldValue + Fuzz * 2)
			return (OldValue + NewValue) / 2;
	}

	return NewValue;

}
