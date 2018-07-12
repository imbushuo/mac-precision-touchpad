#include "driver.h"
#include "Input.tmh"

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineWorker(
	WDFDEVICE Device
)
{
	NTSTATUS Status;
	PDEVICE_CONTEXT pDeviceContext;
	WDF_REQUEST_REUSE_PARAMS ReuseParams;
	WDF_OBJECT_ATTRIBUTES RequestAttributes;
	BOOLEAN RequestStatus;

	PAGED_CODE();

	pDeviceContext = DeviceGetContext(Device);

	if (!pDeviceContext->InitialCompleted)
	{
		WDF_OBJECT_ATTRIBUTES_INIT(&RequestAttributes);
		RequestAttributes.ParentObject = pDeviceContext->SpiDevice;

		Status = WdfRequestCreate(
			&RequestAttributes,
			pDeviceContext->SpiTrackpadIoTarget,
			&pDeviceContext->SpiHidReadRequest
		);

		if (!NT_SUCCESS(Status))
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! WdfRequestCreate fails, status = %!STATUS!",
				Status
			);

			KdPrintEx((
				DPFLTR_IHVDRIVER_ID,
				DPFLTR_INFO_LEVEL,
				"WdfRequestCreate fails, status = 0x%x \n",
				Status
			));

			pDeviceContext->DelayedRequest = TRUE;
			return;
		}

		pDeviceContext->InitialCompleted = TRUE;
	}
	else
	{
		WDF_REQUEST_REUSE_PARAMS_INIT(
			&ReuseParams,
			WDF_REQUEST_REUSE_NO_FLAGS,
			STATUS_NOT_SUPPORTED
		);

		Status = WdfRequestReuse(
			pDeviceContext->SpiHidReadRequest,
			&ReuseParams
		);

		if (!NT_SUCCESS(Status))
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! WdfRequestReuse fails, status = %!STATUS!",
				Status
			);

			KdPrintEx((
				DPFLTR_IHVDRIVER_ID,
				DPFLTR_INFO_LEVEL,
				"WdfRequestReuse fails, status = 0x%x \n",
				Status
				));

			pDeviceContext->DelayedRequest = TRUE;
			return;
		}
	}

	// Invoke HID read request to the device.
	Status = WdfIoTargetFormatRequestForInternalIoctl(
		pDeviceContext->SpiTrackpadIoTarget,
		pDeviceContext->SpiHidReadRequest,
		IOCTL_HID_READ_REPORT,
		pDeviceContext->SpiHidReadBuffer,
		0,
		pDeviceContext->SpiHidReadBuffer,
		0
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! WdfIoTargetFormatRequestForInternalIoctl fails, status = %!STATUS!",
			Status
		);

		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"WdfIoTargetFormatRequestForInternalIoctl fails, status = 0x%x \n",
			Status
		));

		pDeviceContext->DelayedRequest = TRUE;
		return;
	}

	WdfRequestSetCompletionRoutine(
		pDeviceContext->SpiHidReadRequest,
		AmtPtpRequestCompletionRoutine,
		pDeviceContext
	);

	RequestStatus = WdfRequestSend(
		pDeviceContext->SpiHidReadRequest,
		pDeviceContext->SpiTrackpadIoTarget,
		NULL
	);

	pDeviceContext->DelayedRequest = !RequestStatus;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpRequestCompletionRoutine(
	WDFREQUEST Request,
	WDFIOTARGET Target,
	PWDF_REQUEST_COMPLETION_PARAMS Params,
	WDFCONTEXT Context
)
{
	NTSTATUS Status;
	PDEVICE_CONTEXT pDeviceContext;

	LONG SpiRequestLength;
	PSPI_TRACKPAD_PACKET pSpiTrackpadPacket;

	WDFREQUEST PtpRequest;
	PTP_REPORT PtpReport;
	WDFMEMORY PtpRequestMemory;

	LARGE_INTEGER CurrentCounter;
	LONGLONG CounterDelta;

	PAGED_CODE();
	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Params);

	// Get context
	pDeviceContext = (PDEVICE_CONTEXT) Context;

	// Wait
	KeWaitForSingleObject(
		&pDeviceContext->PtpRequestRoutineEvent,
		Executive,
		KernelMode,
		FALSE,
		NULL
	);

	// Clear event
	KeClearEvent(
		&pDeviceContext->PtpRequestRoutineEvent
	);

	// Read report and fulfill PTP request (if have)
	// Not needed to re-format the WDF request
	// timer worker will do that

	Status = WdfIoQueueRetrieveNextRequest(
		pDeviceContext->HidIoQueue,
		&PtpRequest
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! No pending PTP request. Routine exit"
		);

		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"No pending PTP request. Routine exit \n"
		));

		goto set_event;
	}

	SpiRequestLength = (LONG)WdfRequestGetInformation(Request);
	pSpiTrackpadPacket = (PSPI_TRACKPAD_PACKET) WdfMemoryGetBuffer(pDeviceContext->SpiHidReadBuffer, NULL);

	// Get Counter
	KeQueryPerformanceCounter(
		&CurrentCounter
	);

	CounterDelta = (CurrentCounter.QuadPart - pDeviceContext->LastReportTime.QuadPart) / 100;
	pDeviceContext->LastReportTime.QuadPart = CurrentCounter.QuadPart;

	// Write report
	PtpReport.ReportID = REPORTID_MULTITOUCH;
	PtpReport.ContactCount = pSpiTrackpadPacket->NumOfFingers;
	PtpReport.IsButtonClicked = pSpiTrackpadPacket->ClickOccurred;

	UINT8 AdjustedCount = (pSpiTrackpadPacket->NumOfFingers > 5) ? 5 : pSpiTrackpadPacket->NumOfFingers;
	for (UINT8 Count = 0; Count < AdjustedCount; Count++)
	{
		PtpReport.Contacts[Count].ContactID = Count;
		PtpReport.Contacts[Count].X = (USHORT) (pSpiTrackpadPacket->Fingers[Count].X + 5087);
		PtpReport.Contacts[Count].Y = (USHORT) (6089 - pSpiTrackpadPacket->Fingers[Count].Y);
		PtpReport.Contacts[Count].TipSwitch = (pSpiTrackpadPacket->Fingers[Count].Pressure > 0) ? 1 : 0;
		PtpReport.Contacts[Count].Confidence = (pSpiTrackpadPacket->Fingers[Count].TouchMajor < 2500 &&
			pSpiTrackpadPacket->Fingers[Count].TouchMinor < 2500) ? 1 : 0;
	}

	if (CounterDelta >= 0xFF)
	{
		PtpReport.ScanTime = 0xFF;
	}
	else
	{
		PtpReport.ScanTime = (USHORT) CounterDelta;
	}

	Status = WdfRequestRetrieveOutputMemory(
		Request,
		&PtpRequestMemory
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!",
			Status
		);

		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"WdfRequestRetrieveOutputBuffer failed, status = 0x%x \n",
			Status
			));

		goto exit;
	}

	Status = WdfMemoryCopyFromBuffer(
		PtpRequestMemory,
		0,
		(PVOID)&PtpReport,
		sizeof(PTP_REPORT)
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
			Status
		);

		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"WdfMemoryCopyFromBuffer failed, status = 0x%x \n",
			Status
			));

		goto exit;
	}

	// Set information
	WdfRequestSetInformation(
		Request,
		sizeof(PTP_REPORT)
	);

exit:
	WdfRequestComplete(
		Request,
		Status
	);

set_event:

	// Set event
	KeSetEvent(
		&pDeviceContext->PtpRequestRoutineEvent, 
		0, 
		FALSE
	);

	// Invoke next request; not really care about the result
	AmtPtpSpiInputRoutineWorker(
		pDeviceContext->SpiDevice
	);
}
