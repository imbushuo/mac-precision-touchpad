// Hid.c: HID-related routine

#include "Driver.h"
#include "Hid.h"
#include "Hid.tmh"

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

	NTSTATUS               status         = STATUS_SUCCESS;
	PWSTR                  pwstrID;
	size_t                 lenID;
	WDF_REQUEST_PARAMETERS params;
	PDEVICE_CONTEXT        pContext       = DeviceGetContext(Device);
	void                   *pStringBuffer = NULL;

	WDF_REQUEST_PARAMETERS_INIT(&params);
	WdfRequestGetParameters(Request, &params);

	if (pContext->DeviceDescriptor.idProduct != USB_DEVICE_ID_APPLE_MAGICTRACKPAD2)
	{
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "Device HID registry is not found\n");
		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	switch ((ULONG_PTR) params.Parameters.DeviceIoControl.Type3InputBuffer & 0xFFFF)
	{
		case HID_STRING_ID_IMANUFACTURER:
			pwstrID = L"Apple Inc.\0";
			break;
		case HID_STRING_ID_IPRODUCT:
			pwstrID = L"Apple Magic Trackpad 2\0";
			break;
		case HID_STRING_ID_ISERIALNUMBER:
			pwstrID = L"CC27272033DJ2XQAG\0";
			break;
		default:
			pwstrID = NULL;
			break;
	}

	lenID = pwstrID ? wcslen(pwstrID) * sizeof(WCHAR) + sizeof(UNICODE_NULL) : 0;
	if (pwstrID == NULL)
	{
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_DRIVER, "%!FUNC! gets invalid string type\n");
		status = STATUS_INVALID_PARAMETER;
		return status;
	}

	status = WdfRequestRetrieveOutputBuffer(Request, lenID, &pStringBuffer, &lenID);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
		return status;
	}

	RtlCopyMemory(pStringBuffer, pwstrID, lenID);
	WdfRequestSetInformation(Request, lenID);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
	return status;
}