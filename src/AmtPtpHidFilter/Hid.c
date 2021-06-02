// Hid.c: HID specific routines

#include <Driver.h>
#include "Hid.tmh"

NTSTATUS
PtpFilterGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS        status = STATUS_SUCCESS;
	PDEVICE_CONTEXT deviceContext;
	size_t			hidDescriptorSize = 0;
	WDFMEMORY       requestMemory;
	PHID_DESCRIPTOR pSelectedHidDescriptor = NULL;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	status = WdfRequestRetrieveOutputMemory(Request, &requestMemory);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		goto exit;
	}

	switch (deviceContext->ProductID) {
		case HID_PID_MAGIC_TRACKPAD_2:
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Request HID Report Descriptor for Apple Magic Trackpad 2 Family");
			hidDescriptorSize = PtpDefaultHidDescriptorMagicTrackpad2.bLength;
			pSelectedHidDescriptor = &PtpDefaultHidDescriptorMagicTrackpad2;
			break;
		}
	}

	if (pSelectedHidDescriptor != NULL && hidDescriptorSize > 0) {
		status = WdfMemoryCopyFromBuffer(requestMemory, 0, (PVOID)pSelectedHidDescriptor, hidDescriptorSize);
		if (!NT_SUCCESS(status)) {
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
			goto exit;
		}

		WdfRequestSetInformation(Request, hidDescriptorSize);
	}
	else {
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_HID, "%!FUNC! Device HID registry is not found");
		status = STATUS_NOT_SUPPORTED;
		goto exit;
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}

NTSTATUS
PtpFilterGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS               status = STATUS_SUCCESS;
	PDEVICE_CONTEXT        deviceContext;
	PHID_DEVICE_ATTRIBUTES pDeviceAttributes = NULL;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	status = WdfRequestRetrieveOutputBuffer(Request, sizeof(HID_DEVICE_ATTRIBUTES), &pDeviceAttributes, NULL);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		goto exit;
	}

	pDeviceAttributes->Size = sizeof(HID_DEVICE_ATTRIBUTES);
	// Okay here's one thing: we cannot report the real ID here, otherwise there's will be some great conflict with the USB/BT driver.
	// Therefore Vendor ID is changed to a hardcoded number
	pDeviceAttributes->ProductID = deviceContext->ProductID;
	pDeviceAttributes->VendorID = DEVICE_VID;
	pDeviceAttributes->VersionNumber = DEVICE_VERSION;
	WdfRequestSetInformation(Request, sizeof(HID_DEVICE_ATTRIBUTES));

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}

NTSTATUS
PtpFilterGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS               status = STATUS_SUCCESS;
	PDEVICE_CONTEXT        deviceContext;
	size_t			       hidDescriptorSize = 0;
	WDFMEMORY              requestMemory;
	PHID_REPORT_DESCRIPTOR selectedHidDescriptor = NULL;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	status = WdfRequestRetrieveOutputMemory(Request, &requestMemory);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", status);
		goto exit;
	}

	switch (deviceContext->ProductID) {
		case HID_PID_MAGIC_TRACKPAD_2:
		{
			hidDescriptorSize = PtpDefaultHidDescriptorMagicTrackpad2.DescriptorList[0].wReportLength;
			selectedHidDescriptor = PtpReportDescriptorMagicTrackpad2;
			break;
		}
	}

	if (selectedHidDescriptor != NULL && hidDescriptorSize > 0) {
		status = WdfMemoryCopyFromBuffer(requestMemory, 0, (PVOID)selectedHidDescriptor, hidDescriptorSize);
		if (!NT_SUCCESS(status)) {
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", status);
			goto exit;
		}

		WdfRequestSetInformation(Request, hidDescriptorSize);
	}
	else {
		TraceEvents(TRACE_LEVEL_WARNING, TRACE_HID, "%!FUNC! Device HID registry is not found");
		status = STATUS_NOT_SUPPORTED;
		goto exit;
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}

NTSTATUS
PtpFilterGetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN* Pending
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT deviceContext;
	BOOLEAN requestSent;
	WDF_REQUEST_SEND_OPTIONS sendOptions;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	// Forward the IRP to our upstream IO target
	// We don't really care about the content
	WdfRequestFormatRequestUsingCurrentType(Request);
	WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

	// This IOCTL is METHOD_NEITHER, so we just send it without IRP modification
	requestSent = WdfRequestSend(Request, deviceContext->HidIoTarget, &sendOptions);
	*Pending = TRUE;

	if (!requestSent)
	{
		status = WdfRequestGetStatus(Request);
		*Pending = FALSE;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}

NTSTATUS
PtpFilterGetHidFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT deviceContext;
	
	WDF_REQUEST_PARAMETERS requestParameters;
	size_t reportSize;
	PHID_XFER_PACKET hidContent;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&requestParameters);
	WdfRequestGetParameters(Request, &requestParameters);
	if (requestParameters.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
	{
		status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	hidContent = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (hidContent == NULL)
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (hidContent->reportId)
	{
	case REPORTID_DEVICE_CAPS:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_DEVICE_CAPS is requested");

		// Size sanity check
		reportSize = sizeof(PTP_DEVICE_CAPS_FEATURE_REPORT);
		if (hidContent->reportBufferLen < reportSize) {
			status = STATUS_INVALID_BUFFER_SIZE;
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! Report buffer is too small");
			goto exit;
		}

		PPTP_DEVICE_CAPS_FEATURE_REPORT capsReport = (PPTP_DEVICE_CAPS_FEATURE_REPORT)hidContent->reportBuffer;
		capsReport->MaximumContactPoints = PTP_MAX_CONTACT_POINTS;
		capsReport->ButtonType = PTP_BUTTON_TYPE_CLICK_PAD;
		capsReport->ReportID = REPORTID_DEVICE_CAPS;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_DEVICE_CAPS has maximum contact points of %d", capsReport->MaximumContactPoints);
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_DEVICE_CAPS has touchpad type %d", capsReport->ButtonType);
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled");
		break;
	}
	case REPORTID_PTPHQA:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_PTPHQA is requested");

		// Size sanity check
		reportSize = sizeof(PTP_DEVICE_HQA_CERTIFICATION_REPORT);
		if (hidContent->reportBufferLen < reportSize)
		{
			status = STATUS_INVALID_BUFFER_SIZE;
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_HID, "%!FUNC! Report buffer is too small.");
			goto exit;
		}

		PPTP_DEVICE_HQA_CERTIFICATION_REPORT certReport = (PPTP_DEVICE_HQA_CERTIFICATION_REPORT)hidContent->reportBuffer;
		*certReport->CertificationBlob = DEFAULT_PTP_HQA_BLOB;
		certReport->ReportID = REPORTID_PTPHQA;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_PTPHQA is fulfilled");
		break;
	}
	default:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Unsupported type %d is requested", hidContent->reportId);
		status = STATUS_NOT_SUPPORTED;
		goto exit;
	}
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}

NTSTATUS
PtpFilterSetHidFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_CONTEXT deviceContext;

	PHID_XFER_PACKET hidPacket;
	WDF_REQUEST_PARAMETERS requestParameters;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Entry");
	deviceContext = PtpFilterGetContext(Device);

	WDF_REQUEST_PARAMETERS_INIT(&requestParameters);
	WdfRequestGetParameters(Request, &requestParameters);
	if (requestParameters.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET))
	{
		status = STATUS_BUFFER_TOO_SMALL;
		goto exit;
	}

	hidPacket = (PHID_XFER_PACKET)WdfRequestWdmGetIrp(Request)->UserBuffer;
	if (hidPacket == NULL)
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
		goto exit;
	}

	switch (hidPacket->reportId)
	{
	case REPORTID_REPORTMODE:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_REPORTMODE is requested");

		PPTP_DEVICE_INPUT_MODE_REPORT DeviceInputMode = (PPTP_DEVICE_INPUT_MODE_REPORT)hidPacket->reportBuffer;
		switch (DeviceInputMode->Mode)
		{
		case PTP_COLLECTION_MOUSE:
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_REPORTMODE requested Mouse Input");
			deviceContext->PtpInputOn = FALSE;
			break;
		}
		case PTP_COLLECTION_WINDOWS:
		{

			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_REPORTMODE requested Windows PTP Input");
			deviceContext->PtpInputOn = TRUE;
			break;
		}
		}

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_REPORTMODE is fulfilled");
		break;
	}
	case REPORTID_FUNCSWITCH:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_FUNCSWITCH is requested");

		PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT InputSelection = (PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT)hidPacket->reportBuffer;
		deviceContext->PtpReportButton = InputSelection->ButtonReport;
		deviceContext->PtpReportTouch = InputSelection->SurfaceReport;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_FUNCSWITCH requested Button = %d, Surface = %d",
			InputSelection->ButtonReport, InputSelection->SurfaceReport);
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Report REPORTID_FUNCSWITCH is fulfilled");
		break;
	}
	default:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Unsupported type %d is requested", hidPacket->reportId);
		status = STATUS_NOT_SUPPORTED;
		goto exit;
	}
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_HID, "%!FUNC! Exit");
	return status;
}
