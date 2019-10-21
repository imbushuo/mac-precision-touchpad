// Hid.c: HID-related routine

#include "Driver.h"
#include "Hid.tmh"

#ifndef __AAPL_HID_DESCRIPTOR_H__
#define __AAPL_HID_DESCRIPTOR_H__

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily1ReportDescriptor[] = {
	AAPL_SPI_SERIES1_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily1TouchscreenReportDescriptor[] = {
	AAPL_SPI_SERIES1_TOUCHSCREEN_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

CONST HID_DESCRIPTOR AmtPtpSpiFamily1DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,										// bDescriptorType
		sizeof(AmtPtpSpiFamily1ReportDescriptor)    // bDescriptorLength
	}
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily2ReportDescriptor[] = {
	AAPL_SPI_SERIES2_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily2TouchscreenReportDescriptor[] = {
	AAPL_SPI_SERIES2_TOUCHSCREEN_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

CONST HID_DESCRIPTOR AmtPtpSpiFamily2DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,										// bDescriptorType
		sizeof(AmtPtpSpiFamily2ReportDescriptor)    // bDescriptorLength
	}
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily3aReportDescriptor[] = {
	AAPL_SPI_SERIES3_13_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily3aTouchscreenReportDescriptor[] = {
	AAPL_SPI_SERIES3_13_TOUCHSCREEN_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

CONST HID_DESCRIPTOR AmtPtpSpiFamily3aDefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,										// bDescriptorType
		sizeof(AmtPtpSpiFamily3aReportDescriptor)    // bDescriptorLength
	}
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily3bReportDescriptor[] = {
	AAPL_SPI_SERIES3_15_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtpSpiFamily3bTouchscreenReportDescriptor[] = {
	AAPL_SPI_SERIES3_15_TOUCHSCREEN_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

CONST HID_DESCRIPTOR AmtPtpSpiFamily3bDefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,										// bDescriptorType
		sizeof(AmtPtpSpiFamily3bReportDescriptor)    // bDescriptorLength
	}
};

#endif

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	WDFMEMORY RequestMemory;
	size_t CopiedSize = 0;
	PDEVICE_CONTEXT pDeviceContext;
	BOOLEAN DescriptorFound = TRUE;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	pDeviceContext = DeviceGetContext(Device);
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

		goto exit;
	}

	// Get HID descriptor from registry
	switch (pDeviceContext->HidProductID)
	{
		// MacBook 9, 10
		case 0x0275:
		case 0x0279:
		// MacBookAir7,2 also use this fallback
		case 0x0290:
		case 0x0291:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Request HID Report Descriptor for Apple SPI Trackpad, Family 1"
			);

			CopiedSize = AmtPtpSpiFamily1DefaultHidDescriptor.bLength;
			Status = WdfMemoryCopyFromBuffer(
				RequestMemory,
				0,
				(PVOID)&AmtPtpSpiFamily1DefaultHidDescriptor,
				CopiedSize
			);

			if (!NT_SUCCESS(Status))
			{
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					Status
				);
				return Status;
			}
			break;
		}
		// MacBookPro 11, 12 (13-inch). 15-inch is USB trackpad
		case 0x0272:
		case 0x0273:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Request HID Report Descriptor for Apple SPI Trackpad, Family 2"
			);

			CopiedSize = AmtPtpSpiFamily2DefaultHidDescriptor.bLength;
			Status = WdfMemoryCopyFromBuffer(
				RequestMemory,
				0,
				(PVOID)&AmtPtpSpiFamily2DefaultHidDescriptor,
				CopiedSize
			);

			if (!NT_SUCCESS(Status))
			{
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					Status
				);
				return Status;
			}
			break;
		}
		// MacBookPro 13, 14 (13-inch)
		case 0x0276:
		case 0x0277:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Request HID Report Descriptor for Apple SPI Trackpad, Family 3A"
			);

			CopiedSize = AmtPtpSpiFamily3aDefaultHidDescriptor.bLength;
			Status = WdfMemoryCopyFromBuffer(
				RequestMemory,
				0,
				(PVOID)&AmtPtpSpiFamily3aDefaultHidDescriptor,
				CopiedSize
			);

			if (!NT_SUCCESS(Status))
			{
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					Status
				);
				return Status;
			}
			break;
		}
		// MacBookPro 13, 14 (15-inch)
		case 0x0278:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Request HID Report Descriptor for Apple SPI Trackpad, Family 3B"
			);

			CopiedSize = AmtPtpSpiFamily3bDefaultHidDescriptor.bLength;
			Status = WdfMemoryCopyFromBuffer(
				RequestMemory,
				0,
				(PVOID)&AmtPtpSpiFamily3bDefaultHidDescriptor,
				CopiedSize
			);

			if (!NT_SUCCESS(Status))
			{
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					Status
				);
				return Status;
			}
			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Request HID Report Descriptor not found"
			);

			DescriptorFound = FALSE;
			break;
		}
	}

	if (DescriptorFound)
	{
		WdfRequestSetInformation(
			Request,
			CopiedSize
		);
	}
	else
	{
		Status = STATUS_NOT_FOUND;
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pDeviceContext = DeviceGetContext(Device);
	PHID_DEVICE_ATTRIBUTES pDeviceAttributes = NULL;

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	Status = WdfRequestRetrieveOutputBuffer(
		Request,
		sizeof(HID_DEVICE_ATTRIBUTES),
		&pDeviceAttributes,
		NULL
	);

	if (!NT_SUCCESS(Status)) 
	{
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!",
			Status
		);
		
		goto exit;
	}

	pDeviceAttributes->Size = sizeof(HID_DEVICE_ATTRIBUTES);
	pDeviceAttributes->VendorID = pDeviceContext->HidVendorID;
	pDeviceAttributes->ProductID = pDeviceContext->HidProductID;
	pDeviceAttributes->VersionNumber = pDeviceContext->HidVersionNumber;

	WdfRequestSetInformation(
		Request,
		sizeof(HID_DEVICE_ATTRIBUTES)
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
NTSTATUS
AmtPtpGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	size_t CopiedSize = 0;
	WDFMEMORY RequestMemory;
	PDEVICE_CONTEXT pDeviceContext;
	PVOID Descriptor = NULL;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	pDeviceContext = DeviceGetContext(Device);
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
		
		goto exit;
	}

	switch (pDeviceContext->HidProductID)
	{
		// MacBook 9, 10
		case 0x0275:
		case 0x0279:
		// MacBookAir7,2 also use this fallback
		case 0x0290:
		case 0x0291:
		{
			if (pDeviceContext->ReportType == PrecisionTouchpad)
			{
				CopiedSize = AmtPtpSpiFamily1DefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID) &AmtPtpSpiFamily1ReportDescriptor;
			}
			else if (pDeviceContext->ReportType == Touchscreen)
			{
				CopiedSize = AmtPtpSpiFamily1DefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID) &AmtPtpSpiFamily1TouchscreenReportDescriptor;
			}
			break;
		}
		// MacBookPro 11, 12 (13-inch)
		case 0x0272:
		case 0x0273:
		{
			if (pDeviceContext->ReportType == PrecisionTouchpad)
			{
				CopiedSize = AmtPtpSpiFamily2DefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily2ReportDescriptor;
			}
			else if (pDeviceContext->ReportType == Touchscreen)
			{
				CopiedSize = AmtPtpSpiFamily2DefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily2TouchscreenReportDescriptor;
			}
			break;
		}
		// MacBookPro 13, 14 (13-inch)
		case 0x0276:
		case 0x0277:
		{
			if (pDeviceContext->ReportType == PrecisionTouchpad)
			{
				CopiedSize = AmtPtpSpiFamily3aDefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily3aReportDescriptor;
			}
			else if (pDeviceContext->ReportType == Touchscreen)
			{
				CopiedSize = AmtPtpSpiFamily3aDefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily3aTouchscreenReportDescriptor;
			}
			break;
		}
		// MacBookPro 13, 14 (15-inch)
		case 0x0278:
		{
			if (pDeviceContext->ReportType == PrecisionTouchpad)
			{
				CopiedSize = AmtPtpSpiFamily3bDefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily3bReportDescriptor;
			}
			else if (pDeviceContext->ReportType == Touchscreen)
			{
				CopiedSize = AmtPtpSpiFamily3bDefaultHidDescriptor.DescriptorList[0].wReportLength;
				Descriptor = (PVOID)&AmtPtpSpiFamily3bTouchscreenReportDescriptor;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	if (Descriptor == NULL)
	{
		Status = STATUS_NOT_FOUND;
		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_DRIVER,
			"%!FUNC! Device HID descriptor not found"
		);
		return Status;
	}

	if (CopiedSize == 0)
	{
		Status = STATUS_INVALID_DEVICE_STATE;
		TraceEvents(
			TRACE_LEVEL_WARNING,
			TRACE_DRIVER,
			"%!FUNC! Device HID report length is zero"
		);
		return Status;
	}

	Status = WdfMemoryCopyFromBuffer(
		RequestMemory,
		0,
		Descriptor,
		CopiedSize
	);

	if (!NT_SUCCESS(Status)) {

		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
			Status
		);
		return Status;

	}

	WdfRequestSetInformation(
		Request,
		CopiedSize
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
NTSTATUS
AmtPtpGetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN *Pending
)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pDeviceContext;
	BOOLEAN RequestSent;
	WDF_REQUEST_SEND_OPTIONS SendOptions;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! called"
	);

	pDeviceContext = DeviceGetContext(Device);

	// Forward the IRP to our upstream IO target
	// We don't really care about the content
	WdfRequestFormatRequestUsingCurrentType(Request);
	WDF_REQUEST_SEND_OPTIONS_INIT(
		&SendOptions,
		WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET
	);

	RequestSent = WdfRequestSend(
		Request,
		pDeviceContext->SpiTrackpadIoTarget,
		&SendOptions
	);

	*Pending = TRUE;

	if (!RequestSent)
	{
		Status = WdfRequestGetStatus(Request);
		*Pending = FALSE;
	}

	return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpReportFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS Status;
	PDEVICE_CONTEXT pDeviceContext;
	PHID_XFER_PACKET pHidPacket;
	WDF_REQUEST_PARAMETERS RequestParameters;
	size_t ReportSize;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	Status = STATUS_SUCCESS;
	pDeviceContext = DeviceGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&RequestParameters);
	WdfRequestGetParameters(Request, &RequestParameters);

	if (RequestParameters.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
	{
		Status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	pHidPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (pHidPacket == NULL)
	{
		Status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (pHidPacket->reportId)
	{
		case REPORTID_DEVICE_CAPS:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS is requested"
			);

			// Size sanity check
			ReportSize = sizeof(PTP_DEVICE_CAPS_FEATURE_REPORT);
			if (pHidPacket->reportBufferLen < ReportSize) {
				Status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! Report buffer is too small"
				);
				goto exit;
			}

			PPTP_DEVICE_CAPS_FEATURE_REPORT capsReport = (PPTP_DEVICE_CAPS_FEATURE_REPORT) pHidPacket->reportBuffer;

			capsReport->MaximumContactPoints = PTP_MAX_CONTACT_POINTS;
			capsReport->ButtonType = PTP_BUTTON_TYPE_CLICK_PAD;
			capsReport->ReportID = REPORTID_DEVICE_CAPS;

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS has maximum contact points of %d",
				capsReport->MaximumContactPoints
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS has touchpad type %d",
				capsReport->ButtonType
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled"
			);

			break;
		}
		case REPORTID_PTPHQA:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_PTPHQA is requested"
			);

			// Size sanity check
			ReportSize = sizeof(PTP_DEVICE_HQA_CERTIFICATION_REPORT);
			if (pHidPacket->reportBufferLen < ReportSize) 
			{
				Status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR,
					TRACE_DRIVER,
					"%!FUNC! Report buffer is too small."
				);
				goto exit;
			}

			PPTP_DEVICE_HQA_CERTIFICATION_REPORT certReport = (PPTP_DEVICE_HQA_CERTIFICATION_REPORT) pHidPacket->reportBuffer;

			*certReport->CertificationBlob = DEFAULT_PTP_HQA_BLOB;
			certReport->ReportID = REPORTID_PTPHQA;

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_PTPHQA is fulfilled"
			);

			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Unsupported type %d is requested",
				pHidPacket->reportId
			);

			Status = STATUS_NOT_SUPPORTED;
			goto exit;
		}
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS        Status;
	PHID_XFER_PACKET pHidPacket;
	WDF_REQUEST_PARAMETERS RequestParameters;
	PDEVICE_CONTEXT pDeviceContext;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	Status = STATUS_SUCCESS;
	pDeviceContext = DeviceGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&RequestParameters);
	WdfRequestGetParameters(Request, &RequestParameters);

	if (RequestParameters.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET))
	{
		Status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	pHidPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (pHidPacket == NULL)
	{
		Status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (pHidPacket->reportId)
	{
		case REPORTID_REPORTMODE:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_REPORTMODE is requested"
			);

			PPTP_DEVICE_INPUT_MODE_REPORT DeviceInputMode = (PPTP_DEVICE_INPUT_MODE_REPORT) pHidPacket->reportBuffer;
			switch (DeviceInputMode->Mode)
			{
				case PTP_COLLECTION_MOUSE:
				{
					TraceEvents(
						TRACE_LEVEL_INFORMATION,
						TRACE_DRIVER,
						"%!FUNC! Report REPORTID_REPORTMODE requested Mouse Input"
					);

					pDeviceContext->PtpInputOn = FALSE;
					break;
				}
				case PTP_COLLECTION_WINDOWS:
				{

					TraceEvents(
						TRACE_LEVEL_INFORMATION,
						TRACE_DRIVER,
						"%!FUNC! Report REPORTID_REPORTMODE requested Windows PTP Input"
					);

					pDeviceContext->PtpInputOn = TRUE;
					break;
				}
			}

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_REPORTMODE is fulfilled"
			);
			break;
		}
		case REPORTID_FUNCSWITCH:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH is requested"
			);

			PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT InputSelection = (PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT) pHidPacket->reportBuffer;
			pDeviceContext->PtpReportButton = InputSelection->ButtonReport;
			pDeviceContext->PtpReportTouch = InputSelection->SurfaceReport;

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH requested Button = %d, Surface = %d",
				InputSelection->ButtonReport,
				InputSelection->SurfaceReport
			);

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH is fulfilled"
			);

			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER,
				"%!FUNC! Unsupported type %d is requested",
				pHidPacket->reportId
			);

			Status = STATUS_NOT_SUPPORTED;
			goto exit;
		}
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION,
		TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return Status;
}
