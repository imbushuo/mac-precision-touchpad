// Hid.c: HID-related routine

#include "Driver.h"
#include "Hid.tmh"

#ifndef _AAPL_HID_DESCRIPTOR_H_
#define _AAPL_HID_DESCRIPTOR_H_

HID_REPORT_DESCRIPTOR AAPLMagicTrackpad2ReportDescriptor[] = {
	AAPL_MAGIC_TRACKPAD2_PTP_TLC,
	AAPL_PTP_CONFIGURATION_TLC,
	AAPL_MAGIC_TRACKPAD2_MOUSE_TLC
};

CONST HID_DESCRIPTOR AAPLMagicTrackpad2DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                                         // bDescriptorType
		sizeof(AAPLMagicTrackpad2ReportDescriptor)    // bDescriptorLength
	}
};
#endif

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2GetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	NTSTATUS        status   = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = DeviceGetContext(Device);
	size_t			szCopy   = 0;
	WDFMEMORY       reqMemory;

	status = WdfRequestRetrieveOutputMemory(Request, &reqMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		return status;
	}

	if (pContext->DeviceDescriptor.idProduct == USB_DEVICE_ID_APPLE_MAGICTRACKPAD2)
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Request HID Report Descriptor for AAPL Magic Trackpad 2");
		szCopy = AAPLMagicTrackpad2DefaultHidDescriptor.bLength;

		status = WdfMemoryCopyFromBuffer(reqMemory, 0, (PVOID)&AAPLMagicTrackpad2DefaultHidDescriptor, szCopy);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
			return status;
		}

		WdfRequestSetInformation(Request, szCopy);
	}
	else
	{
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "Device HID registry is not found");
		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2GetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	NTSTATUS               status            = STATUS_SUCCESS;
	PDEVICE_CONTEXT        pContext          = DeviceGetContext(Device);
	PHID_DEVICE_ATTRIBUTES pDeviceAttributes = NULL;

	status = WdfRequestRetrieveOutputBuffer(Request, sizeof(HID_DEVICE_ATTRIBUTES), 
		&pDeviceAttributes, NULL);

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		return status;
	}

	pDeviceAttributes->Size          = sizeof(HID_DEVICE_ATTRIBUTES);
	pDeviceAttributes->ProductID     = pContext->DeviceDescriptor.idProduct;
	pDeviceAttributes->VendorID      = pContext->DeviceDescriptor.idVendor;
	pDeviceAttributes->VersionNumber = DEVICE_VERSION;
	
	// Set length
	WdfRequestSetInformation(Request, sizeof(HID_DEVICE_ATTRIBUTES));
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2GetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	NTSTATUS               status            = STATUS_SUCCESS;
	PDEVICE_CONTEXT        pContext          = DeviceGetContext(Device);
	size_t			       szCopy            = 0;
	WDFMEMORY              reqMemory;

	status = WdfRequestRetrieveOutputMemory(Request, &reqMemory);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		return status;
	}

	if (pContext->DeviceDescriptor.idProduct == USB_DEVICE_ID_APPLE_MAGICTRACKPAD2)
	{
		szCopy = AAPLMagicTrackpad2DefaultHidDescriptor.DescriptorList[0].wReportLength;
		if (szCopy == 0)
		{
			status = STATUS_INVALID_DEVICE_STATE;
			TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "Device HID report length is zero\n");
			return status;
		}

		status = WdfMemoryCopyFromBuffer(reqMemory, 0, (PVOID)&AAPLMagicTrackpad2ReportDescriptor, szCopy);
		if (!NT_SUCCESS(status))
		{
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
			return status;
		}

		WdfRequestSetInformation(Request, szCopy);
	}
	else
	{
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "Device HID registry is not found\n");
		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
MagicTrackpad2GetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	NTSTATUS               status = STATUS_SUCCESS;
	WDF_REQUEST_PARAMETERS params;
	PDEVICE_CONTEXT        pContext = DeviceGetContext(Device);
	void                   *pStringBuffer = NULL;
	WDFMEMORY              memHandle;
	USHORT                 wcharCount;
	size_t                 actualSize;
	UCHAR                  strIndex;

	WDF_REQUEST_PARAMETERS_INIT(&params);
	WdfRequestGetParameters(Request, &params);

	switch ((ULONG_PTR) params.Parameters.DeviceIoControl.Type3InputBuffer & 0xFFFF)
	{
		case HID_STRING_ID_IMANUFACTURER:
			strIndex = pContext->DeviceDescriptor.iManufacturer;
			break;
		case HID_STRING_ID_IPRODUCT:
			strIndex = pContext->DeviceDescriptor.iProduct;
			break;
		case HID_STRING_ID_ISERIALNUMBER:
			strIndex = pContext->DeviceDescriptor.iSerialNumber;
			break;
		default:
			TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "%!FUNC! gets invalid string type\n");
			// Return product name as default
			strIndex = pContext->DeviceDescriptor.iProduct;
			return status;
	}

	status = WdfUsbTargetDeviceAllocAndQueryString(pContext->UsbDevice, 
		WDF_NO_OBJECT_ATTRIBUTES, &memHandle, &wcharCount, strIndex, 0x409);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfUsbTargetDeviceAllocAndQueryString failed with %!STATUS!", status);
		return status;
	}

	status = WdfRequestRetrieveOutputBuffer(Request, wcharCount * sizeof(WCHAR), &pStringBuffer, &actualSize);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		return status;
	}

	WdfMemoryCopyToBuffer(memHandle, 0, &pStringBuffer, actualSize);
	WdfRequestSetInformation(Request, actualSize);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}

NTSTATUS
RequestGetHidXferPacketToReadFromDevice(
	_In_  WDFREQUEST        Request,
	_Out_ HID_XFER_PACKET  *Packet
)
{
	//
	// Driver need to write to the output buffer (so that App can read from it)
	//
	//   Report Buffer: Output Buffer
	//   Report Id    : Input Buffer
	//

	NTSTATUS                status;
	WDFMEMORY               inputMemory;
	WDFMEMORY               outputMemory;
	size_t                  inputBufferLength;
	size_t                  outputBufferLength;
	PVOID                   inputBuffer;
	PVOID                   outputBuffer;

	//
	// Get report Id from input buffer
	//
	status = WdfRequestRetrieveInputMemory(Request, &inputMemory);
	if (!NT_SUCCESS(status)) {
		KdPrint(("WdfRequestRetrieveInputMemory failed 0x%x\n", status));
		return status;
	}
	inputBuffer = WdfMemoryGetBuffer(inputMemory, &inputBufferLength);

	if (inputBufferLength < sizeof(UCHAR)) {
		status = STATUS_INVALID_BUFFER_SIZE;
		KdPrint(("WdfRequestRetrieveInputMemory: invalid input buffer. size %d, expect %d\n",
			(int)inputBufferLength, (int)sizeof(UCHAR)));
		return status;
	}

	Packet->reportId = *(PUCHAR)inputBuffer;

	//
	// Get report buffer from output buffer
	//
	status = WdfRequestRetrieveOutputMemory(Request, &outputMemory);
	if (!NT_SUCCESS(status)) {
		KdPrint(("WdfRequestRetrieveOutputMemory failed 0x%x\n", status));
		return status;
	}

	outputBuffer = WdfMemoryGetBuffer(outputMemory, &outputBufferLength);

	Packet->reportBuffer = (PUCHAR)outputBuffer;
	Packet->reportBufferLen = (ULONG)outputBufferLength;

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
	PDEVICE_CONTEXT deviceContext;
	HID_XFER_PACKET packet;
	size_t reportSize;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	status = STATUS_SUCCESS;
	deviceContext = DeviceGetContext(Device);

	status = RequestGetHidXferPacketToReadFromDevice(Request, &packet);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! RequestGetHidXferPacketToReadFromDevice failed with status %!STATUS!", status);
		goto exit;
	}

	switch (packet.reportId)
	{
		case REPORTID_DEVICE_CAPS:
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Report REPORTID_DEVICE_CAPS is requested.\n");
			reportSize = sizeof(PPTP_DEVICE_CAPS_FEATURE_REPORT) + sizeof(packet.reportId);
			// Size sanity check
			if (packet.reportBufferLen < reportSize) 
			{
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Report buffer is too small.\n");
				goto exit;
			}

			PPTP_DEVICE_CAPS_FEATURE_REPORT capsReport = (PPTP_DEVICE_CAPS_FEATURE_REPORT) (packet.reportBuffer + sizeof(packet.reportId));

			capsReport->DeviceCaps.MaximumContactPoints = PTP_MAX_CONTACT_POINTS;
			capsReport->DeviceCaps.ButtonType = PTP_BUTTON_TYPE_CLICK_PAD;

			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled.\n");
			WdfRequestSetInformation(Request, reportSize);
			break;
		}
		case REPORTID_PTPHQA:
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Report REPORTID_PTPHQA is requested.\n");
			reportSize = sizeof(PPTP_DEVICE_HQA_CERTIFICATION_REPORT) + sizeof(packet.reportId);
			// Size sanity check
			if (packet.reportBufferLen < reportSize)
			{
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Report buffer is too small.\n");
				goto exit;
			}

			PPTP_DEVICE_HQA_CERTIFICATION_REPORT certReport = (PPTP_DEVICE_HQA_CERTIFICATION_REPORT) (packet.reportBuffer + sizeof(packet.reportId));
			*certReport->CertificationBlob = DEFAULT_PTP_HQA_BLOB;

			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled.\n");
			WdfRequestSetInformation(Request, reportSize);
			break;
		}
		default:
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Unsupported type %d is requested.\n", packet.reportId);
			status = STATUS_NOT_SUPPORTED;
			goto exit;
	}
exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}