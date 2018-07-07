#include "driver.h"
#include "Input.tmh"

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineWorker(
	WDFDEVICE Device
)
{
	NTSTATUS Status;
	WDFREQUEST Request;
	PDEVICE_CONTEXT pDeviceContext;

	PAGED_CODE();
	pDeviceContext = DeviceGetContext(Device);

	// Get PTP report
	Status = WdfIoQueueRetrieveNextRequest(
		pDeviceContext->HidIoQueue,
		&Request
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

		return;
	}

	AmtPtpSpiInputRoutineInternal(
		Device,
		Request
	);
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
AmtPtpSpiInputRoutineInternal(
	WDFDEVICE Device,
	WDFREQUEST Request
)
{
	NTSTATUS Status;
	WDFMEMORY InputBuffer;
	WDFMEMORY RequestMemory;
	WDF_MEMORY_DESCRIPTOR InputBufferDescriptor;
	LARGE_INTEGER CurrentCounter;
	PDEVICE_CONTEXT pDeviceContext;
	PTP_REPORT PtpReport;
	LONGLONG CounterDelta;

	// Get context
	pDeviceContext = DeviceGetContext(Device);

	// Read device
	Status = WdfMemoryCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		NonPagedPool,
		PTP_POOL_TAG,
		REPORT_BUFFER_SIZE,
		&InputBuffer,
		NULL
	);

	if (!NT_SUCCESS(Status))
	{
		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"Failed to allocate memory, status = 0x%x \n",
			Status
		));

		goto exit;
	}

	WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
		&InputBufferDescriptor,
		InputBuffer,
		0
	);

	Status = WdfIoTargetSendInternalIoctlSynchronously(
		pDeviceContext->SpiTrackpadIoTarget,
		NULL,
		IOCTL_HID_READ_REPORT,
		NULL,
		&InputBufferDescriptor,
		NULL,
		NULL
	);

	if (!NT_SUCCESS(Status))
	{
		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"WdfIoTargetSendInternalIoctlSynchronously failed, status = 0x%x \n",
			Status
		));

		goto exit;
	}
	else
	{
		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"WdfIoTargetSendInternalIoctlSynchronously succeeded \n",
			Status
		));
	}

	// Parse content
	size_t BufferSize = 0;
	PSPI_TRACKPAD_PACKET pSpiTrackpadPacket = (PSPI_TRACKPAD_PACKET) WdfMemoryGetBuffer(
		InputBuffer,
		&BufferSize
	);

	if (pSpiTrackpadPacket == NULL || BufferSize <= 0)
	{
		KdPrintEx((
			DPFLTR_IHVDRIVER_ID,
			DPFLTR_INFO_LEVEL,
			"Invalid pSpiTrackpadPacket pointer \n",
			Status
		));

		goto exit;
	}

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

	KeQueryPerformanceCounter(
		&CurrentCounter
	);

	CounterDelta = (CurrentCounter.QuadPart - pDeviceContext->LastReportTime.QuadPart) / 100;
	pDeviceContext->LastReportTime.QuadPart = CurrentCounter.QuadPart;

	if (CounterDelta >= 0xFF)
	{
		PtpReport.ScanTime = 0xFF;
	}
	else
	{
		PtpReport.ScanTime = (USHORT)CounterDelta;
	}

	Status = WdfRequestRetrieveOutputMemory(
		Request,
		&RequestMemory
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
		RequestMemory,
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
}