#include "Driver.h"

PCHAR
DbgDevicePowerString(
	_In_ WDF_POWER_DEVICE_STATE Type
)
{
	switch (Type)
	{
	case WdfPowerDeviceInvalid:
		return "WdfPowerDeviceInvalid";
	case WdfPowerDeviceD0:
		return "WdfPowerDeviceD0";
	case WdfPowerDeviceD1:
		return "WdfPowerDeviceD1";
	case WdfPowerDeviceD2:
		return "WdfPowerDeviceD2";
	case WdfPowerDeviceD3:
		return "WdfPowerDeviceD3";
	case WdfPowerDeviceD3Final:
		return "WdfPowerDeviceD3Final";
	case WdfPowerDevicePrepareForHibernation:
		return "WdfPowerDevicePrepareForHibernation";
	case WdfPowerDeviceMaximum:
		return "WdfPowerDeviceMaximum";
	default:
		return "UnKnown Device Power State";
	}
}

PCHAR
DbgIoControlGetString(
	_In_ ULONG IoControlCode
)
{

	switch (IoControlCode)
	{
	case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
		return "IOCTL_HID_GET_DEVICE_DESCRIPTOR";
	case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
		return "IOCTL_HID_GET_DEVICE_ATTRIBUTES";
	case IOCTL_HID_GET_REPORT_DESCRIPTOR:
		return "IOCTL_HID_GET_REPORT_DESCRIPTOR";
	case IOCTL_HID_GET_STRING:
		return "IOCTL_HID_GET_STRING";
	case IOCTL_HID_READ_REPORT:
		return "IOCTL_HID_READ_REPORT";
	case IOCTL_HID_WRITE_REPORT:
		return "IOCTL_HID_WRITE_REPORT";
	case IOCTL_UMDF_HID_GET_INPUT_REPORT:
		return "IOCTL_UMDF_HID_GET_INPUT_REPORT";
	case IOCTL_UMDF_HID_SET_OUTPUT_REPORT:
		return "IOCTL_UMDF_HID_SET_OUTPUT_REPORT";
	case IOCTL_UMDF_HID_GET_FEATURE:
		return "IOCTL_UMDF_HID_GET_FEATURE";
	case IOCTL_UMDF_HID_SET_FEATURE:
		return "IOCTL_UMDF_HID_SET_FEATURE";
	case IOCTL_HID_ACTIVATE_DEVICE:
		return "IOCTL_HID_ACTIVATE_DEVICE";
	case IOCTL_HID_DEACTIVATE_DEVICE:
		return "IOCTL_HID_DEACTIVATE_DEVICE";
	case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:
		return "IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST";
	default:
		return "IOCTL_UNKNOWN";
	}
}
