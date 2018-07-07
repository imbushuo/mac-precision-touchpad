// Spb.c: SPI-based trackpad read

#include "driver.h"
#include "Spb.h"
#include "Spb.tmh"

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSpiStartInternalReportReadLoop(
	_In_ WDFDEVICE Device
)
{
	NTSTATUS Status;
	PDEVICE_CONTEXT pDeviceContext;

	WDF_TIMER_CONFIG SpiTimerConfig;
	WDF_OBJECT_ATTRIBUTES SpiTimerAttributes;
	BOOLEAN TimerStatus = FALSE;

	WDFKEY PrimaryKey;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	pDeviceContext = DeviceGetContext(Device);

	// Dump a copy of HID descriptor for further analysis
	Status = WdfDeviceOpenRegistryKey(
		Device,
		PLUGPLAY_REGKEY_DEVICE | WDF_REGKEY_DEVICE_SUBKEY,
		KEY_READ | KEY_SET_VALUE,
		WDF_NO_OBJECT_ATTRIBUTES,
		&PrimaryKey
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfDeviceOpenRegistryKey failed, status = %!STATUS!",
			Status
		);
	}
	else
	{
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! WdfDeviceOpenRegistryKey succeeded"
		);

		WDFMEMORY OutputMemory;
		WDF_MEMORY_DESCRIPTOR OutputMemoryDescriptor;

		Status = WdfMemoryCreate(
			WDF_NO_OBJECT_ATTRIBUTES,
			NonPagedPool,
			0,
			4096,
			&OutputMemory,
			NULL
		);

		WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
			&OutputMemoryDescriptor,
			OutputMemory,
			NULL
		);

		Status = WdfIoTargetSendIoctlSynchronously(
			pDeviceContext->SpiTrackpadIoTarget,
			NULL,
			IOCTL_HID_READ_REPORT,
			NULL,
			&OutputMemoryDescriptor,
			NULL,
			NULL
		);

		if (NT_SUCCESS(Status))
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! WdfIoTargetSendIoctlSynchronously succeeded"
			);

			UNICODE_STRING SampleHidReportName;

			RtlInitUnicodeString(
				&SampleHidReportName,
				L"SampleHIDReport"
			);

			Status = WdfRegistryAssignMemory(
				PrimaryKey,
				&SampleHidReportName,
				REG_BINARY,
				OutputMemory,
				NULL
			);

			if (!NT_SUCCESS(Status))
			{
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! WdfRegistryAssignMemory failed, status = %!STATUS!",
					Status
				);
			}
		}
		else
		{
			TraceEvents(
				TRACE_LEVEL_ERROR,
				TRACE_DRIVER,
				"%!FUNC! WdfIoTargetSendIoctlSynchronously failed, status = %!STATUS!",
				Status
			);
		}
	}

	// Initialize the lock
	Status = WdfWaitLockCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		&pDeviceContext->SpiReadLock
	);

	// Immediate return if failed
	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfWaitLockCreate failed, status = %!STATUS!",
			Status
		);

		goto exit;
	}

	// Fire the timer
	WDF_TIMER_CONFIG_INIT_PERIODIC(&SpiTimerConfig, AmtPtpSpiTimerRoutine, 100);

	WDF_OBJECT_ATTRIBUTES_INIT(&SpiTimerAttributes);
	SpiTimerAttributes.ParentObject = Device;

	Status = WdfTimerCreate(
		&SpiTimerConfig,
		&SpiTimerAttributes,
		&pDeviceContext->SpiTimer
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfTimerCreate failed, status = %!STATUS!",
			Status
		);

		goto exit;
	}

	// For debug purpose right now.
	if (pDeviceContext->SpiTimer != NULL)
	{
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! Starting timer."
		);

		TimerStatus = WdfTimerStart(
			pDeviceContext->SpiTimer,
			WDF_ABS_TIMEOUT_IN_MS(10)
		);
	}
	
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Timer Started."
	);

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
static VOID 
AmtPtpSpiTimerRoutine(
	WDFTIMER Timer
)
{
	NTSTATUS Status;
	WDFDEVICE Device;
	PDEVICE_CONTEXT pDeviceContext = NULL;

	WDFMEMORY OutputMemory;
	WDF_MEMORY_DESCRIPTOR OutputMemoryDescriptor;

	PSPI_TRACKPAD_PACKET TrackpadReport;
	ULONG_PTR BytesReturned;
	size_t ActualReadLength;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	Device = (WDFDEVICE) WdfTimerGetParentObject(Timer);
	pDeviceContext = DeviceGetContext(Device);

	// Try to get the lock
	Status = WdfWaitLockAcquire(
		pDeviceContext->SpiReadLock,
		NULL
	);

	if (!NT_SUCCESS(Status)) return;

	// Try send IOCTL
	Status = WdfMemoryCreate(
		WDF_NO_OBJECT_ATTRIBUTES,
		NonPagedPool,
		0,
		SPI_REPORT_BUFFER_SIZE,
		&OutputMemory,
		NULL
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfMemoryCreate failed, status = %!STATUS!",
			Status
		);

		goto exit_release_lock;
	}

	WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
		&OutputMemoryDescriptor,
		OutputMemory,
		NULL
	);

	Status = WdfIoTargetSendIoctlSynchronously(
		pDeviceContext->SpiTrackpadIoTarget,
		NULL,
		IOCTL_HID_READ_REPORT,
		NULL,
		&OutputMemoryDescriptor,
		NULL,
		&BytesReturned
	);

	if (!NT_SUCCESS(Status))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfIoTargetSendIoctlSynchronously failed, status = %!STATUS!",
			Status
		);

		goto exit_release_lock;
	}

	// Parse returned content
	TrackpadReport = (PSPI_TRACKPAD_PACKET) WdfMemoryGetBuffer(
		OutputMemory,
		&ActualReadLength
	);

	if (TrackpadReport != NULL)
	{
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER,
			"%!FUNC! BytesReturned: 0x%llx, Type: %d, Clicked: %!bool!, IsFinger: %!bool!, NumOfFingers: %d",
			BytesReturned,
			TrackpadReport->PacketType,
			TrackpadReport->ClickOccurred,
			TrackpadReport->IsFinger,
			TrackpadReport->NumOfFingers
		);

		// Write to dump for analysis
		TraceLoggingWrite(
			g_hAmtPtpDeviceTraceProvider,
			EVENT_DRIVER_FUNCTIONAL,
			TraceLoggingString("AmtPtpSpiTimerRoutine", "Routine"),
			TraceLoggingString("WdfIoTargetSendIoctlSynchronously", "Context"),
			TraceLoggingInt32(Status, "Status"),
			TraceLoggingString(EVENT_DRIVER_HID_DUMP, EVENT_DRIVER_FUNC_SUBTYPE),
			TraceLoggingBinary((void*)TrackpadReport, (UINT16) ActualReadLength, "HidContent")
		);
	}

	WdfObjectDelete(
		OutputMemory
	);

exit_release_lock:
	// Finally release the lock
	WdfWaitLockRelease(
		pDeviceContext->SpiReadLock
	);

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);
}