#include "Driver.h"
#include "hid.tmh"

#ifndef _AAPL_HID_DESCRIPTOR_H_
#define _AAPL_HID_DESCRIPTOR_H_

HID_REPORT_DESCRIPTOR AmtPtpT2ReportDescriptor[] = {
	AAPL_WELLSPRING_T2_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
};

CONST HID_DESCRIPTOR AmtPtpT2DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtpT2ReportDescriptor)    // bDescriptorLength
	},
};

#endif

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = DeviceGetContext(Device);
	size_t szCopy = 0;
	WDFMEMORY requestMemory;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputMemory(
		Request,
		&requestMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!",
			status
		);
		goto exit;
	}

	switch (pContext->DeviceDescriptor.idProduct) {
		case USB_DEVICE_ID_APPLE_T2_7A:
		case USB_DEVICE_ID_APPLE_T2_7B:
		case USB_DEVICE_ID_APPLE_T2_7C:
		case USB_DEVICE_ID_APPLE_T2_7D:
		{
			szCopy = AmtPtpT2DefaultHidDescriptor.bLength;
			status = WdfMemoryCopyFromBuffer(
				requestMemory,
				0,
				(PVOID) &AmtPtpT2DefaultHidDescriptor,
				szCopy
			);

			if (!NT_SUCCESS(status)) {
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					status
				);
				goto exit;
			}

			WdfRequestSetInformation(Request,szCopy);
			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_WARNING, TRACE_DRIVER,
				"%!FUNC! Device HID registry is not found, use a generic fallback"
			);

			szCopy = AmtPtpT2DefaultHidDescriptor.bLength;
			status = WdfMemoryCopyFromBuffer(
				requestMemory,
				0,
				(PVOID)&AmtPtpT2DefaultHidDescriptor,
				szCopy
			);

			if (!NT_SUCCESS(status)) {
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					status
				);
				goto exit;
			}

			WdfRequestSetInformation(Request, szCopy);
			break;
		}
	};

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = DeviceGetContext(Device);
	PHID_DEVICE_ATTRIBUTES pDeviceAttributes = NULL;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputBuffer(
		Request,
		sizeof(HID_DEVICE_ATTRIBUTES),
		&pDeviceAttributes,
		NULL
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!",
			status
		);
		goto exit;
	}

	pDeviceAttributes->Size = sizeof(HID_DEVICE_ATTRIBUTES);
	pDeviceAttributes->ProductID = pContext->DeviceDescriptor.idProduct;
	pDeviceAttributes->VendorID = pContext->DeviceDescriptor.idVendor;
	pDeviceAttributes->VersionNumber = DEVICE_VERSION;

	WdfRequestSetInformation(Request, sizeof(HID_DEVICE_ATTRIBUTES));

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = DeviceGetContext(Device);
	size_t szCopy = 0;
	WDFMEMORY requestMemory;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputMemory(
		Request,
		&requestMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!",
			status
		);
		goto exit;
	}

	switch (pContext->DeviceDescriptor.idProduct) {
		case USB_DEVICE_ID_APPLE_T2_7A:
		case USB_DEVICE_ID_APPLE_T2_7B:
		case USB_DEVICE_ID_APPLE_T2_7C:
		case USB_DEVICE_ID_APPLE_T2_7D:
		{

			szCopy = AmtPtpT2DefaultHidDescriptor.DescriptorList[0].wReportLength;
			if (szCopy == 0) {

				status = STATUS_INVALID_DEVICE_STATE;
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! Device HID report length is zero"
				);
				goto exit;
			}

			status = WdfMemoryCopyFromBuffer(
				requestMemory,
				0,
				(PVOID) &AmtPtpT2ReportDescriptor,
				szCopy
			);

			if (!NT_SUCCESS(status)) {

				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					status
				);
				goto exit;
			}

			WdfRequestSetInformation(Request, szCopy);
			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_WARNING, TRACE_DRIVER,
				"%!FUNC! Device HID registry is not found, use a generic fallback"
			);
			
			szCopy = AmtPtpT2DefaultHidDescriptor.DescriptorList[0].wReportLength;
			if (szCopy == 0) {

				status = STATUS_INVALID_DEVICE_STATE;
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! Device HID report length is zero"
				);
				goto exit;
			}

			status = WdfMemoryCopyFromBuffer(
				requestMemory,
				0,
				(PVOID)&AmtPtpT2ReportDescriptor,
				szCopy
			);

			if (!NT_SUCCESS(status)) {

				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!",
					status
				);
				goto exit;
			}

			WdfRequestSetInformation(Request, szCopy);
			break;
		}
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpReportFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status;
	PDEVICE_CONTEXT pDeviceContext;
	PHID_XFER_PACKET pHidPacket;
	WDF_REQUEST_PARAMETERS RequestParameters;
	size_t ReportSize;

	PAGED_CODE();

	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	status = STATUS_SUCCESS;
	pDeviceContext = DeviceGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&RequestParameters);
	WdfRequestGetParameters(Request, &RequestParameters);

	if (RequestParameters.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
	{
		status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	pHidPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (pHidPacket == NULL)
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (pHidPacket->reportId)
	{
		case REPORTID_DEVICE_CAPS:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS is requested"
			);

			// Size sanity check
			ReportSize = sizeof(PTP_DEVICE_CAPS_FEATURE_REPORT);
			if (pHidPacket->reportBufferLen < ReportSize) {
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! Report buffer is too small"
				);
				goto exit;
			}

			PPTP_DEVICE_CAPS_FEATURE_REPORT capsReport = (PPTP_DEVICE_CAPS_FEATURE_REPORT) pHidPacket->reportBuffer;
			capsReport->MaximumContactPoints = PTP_MAX_CONTACT_POINTS;
			capsReport->ButtonType = PTP_BUTTON_TYPE_CLICK_PAD;
			capsReport->ReportID = REPORTID_DEVICE_CAPS;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS has maximum contact points of %d",
				capsReport->MaximumContactPoints
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS has touchpad type %d",
				capsReport->ButtonType
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled"
			);

			break;
		}
		case REPORTID_PTPHQA:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_PTPHQA is requested"
			);

			// Size sanity check
			ReportSize = sizeof(PTP_DEVICE_HQA_CERTIFICATION_REPORT);
			if (pHidPacket->reportBufferLen < ReportSize)
			{
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"%!FUNC! Report buffer is too small."
				);
				goto exit;
			}

			PPTP_DEVICE_HQA_CERTIFICATION_REPORT certReport = (PPTP_DEVICE_HQA_CERTIFICATION_REPORT)pHidPacket->reportBuffer;
			*certReport->CertificationBlob = DEFAULT_PTP_HQA_BLOB;
			certReport->ReportID = REPORTID_PTPHQA;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_PTPHQA is fulfilled"
			);

			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Unsupported type %d is requested",
				pHidPacket->reportId
			);

			status = STATUS_NOT_SUPPORTED;
			goto exit;
		}
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Exit"
	);

	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS        status;
	PHID_XFER_PACKET pHidPacket;
	WDF_REQUEST_PARAMETERS RequestParameters;
	PDEVICE_CONTEXT pDeviceContext;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Entry"
	);

	status = STATUS_SUCCESS;
	pDeviceContext = DeviceGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&RequestParameters);
	WdfRequestGetParameters(Request, &RequestParameters);

	if (RequestParameters.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET))
	{
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! HID_XFER_PACKET buffer too small"
		);

		status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	pHidPacket = (PHID_XFER_PACKET) WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (pHidPacket == NULL)
	{
		TraceEvents(
			TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"%!FUNC! HID_XFER_PACKET has no input packet"
		);

		status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (pHidPacket->reportId)
	{
		case REPORTID_REPORTMODE:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_REPORTMODE is requested"
			);

			PPTP_DEVICE_INPUT_MODE_REPORT devInputMode = (PPTP_DEVICE_INPUT_MODE_REPORT) pHidPacket->reportBuffer;
			BOOLEAN bWellspringMode = pDeviceContext->IsWellspringModeOn;

			switch (devInputMode->Mode)
			{
				case PTP_COLLECTION_MOUSE:
				{
					TraceEvents(
						TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
						"%!FUNC! Report REPORTID_REPORTMODE requested Mouse Input but not supported"
					);

					status = STATUS_NOT_SUPPORTED;
					goto exit;
				}
				case PTP_COLLECTION_WINDOWS:
				{
					TraceEvents(
						TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
						"%!FUNC! Report REPORTID_REPORTMODE requested Windows PTP Input"
					);

					if (!bWellspringMode) {
						status = AmtPtpSetWellspringMode(pDeviceContext, TRUE);
						if (!NT_SUCCESS(status)) {
							TraceEvents(
								TRACE_LEVEL_ERROR, TRACE_DRIVER,
								"%!FUNC! -> AmtPtpSetWellspringMode failed with status %!STATUS!",
								status
							);
							goto exit;
						}
					}
					break;
				}
			}

			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_REPORTMODE is fulfilled"
			);
			break;
		}
		case REPORTID_FUNCSWITCH:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH is requested"
			);

			PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT secInput = (PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT) pHidPacket->reportBuffer;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH requested Button = %d, Surface = %d",
				secInput->ButtonReport,
				secInput->SurfaceReport
			);

			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Report REPORTID_FUNCSWITCH is fulfilled"
			);
			break;
		}
		default:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
				"%!FUNC! Unsupported type %d is requested",
				pHidPacket->reportId
			);
			status = STATUS_NOT_SUPPORTED;
			goto exit;
		}
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, TRACE_DRIVER,
		"%!FUNC! Exit"
	);
	return status;
}
