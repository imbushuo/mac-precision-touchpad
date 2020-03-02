#pragma once

#ifndef _AAPL_HID_DESCRIPTOR_H_
#define _AAPL_HID_DESCRIPTOR_H_

HID_REPORT_DESCRIPTOR AmtPtp3ReportDescriptor[] = {
	AAPL_WELLSPRING_3_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtp5ReportDescriptor[] = {
	AAPL_WELLSPRING_5_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtp6ReportDescriptor[] = {
	AAPL_WELLSPRING_6_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtp7aReportDescriptor[] = {
	AAPL_WELLSPRING_7A_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtp8ReportDescriptor[] = {
	AAPL_WELLSPRING_8_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_REPORT_DESCRIPTOR AmtPtpMt2ReportDescriptor[] = {
	AAPL_MAGIC_TRACKPAD2_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
	AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC
};

HID_DESCRIPTOR AmtPtp3DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtp3ReportDescriptor)    // bDescriptorLength
	},
};

HID_DESCRIPTOR AmtPtp5DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtp5ReportDescriptor)    // bDescriptorLength
	},
};

HID_DESCRIPTOR AmtPtp6DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtp6ReportDescriptor)    // bDescriptorLength
	},
};

HID_DESCRIPTOR AmtPtp7aDefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtp7aReportDescriptor)    // bDescriptorLength
	},
};

HID_DESCRIPTOR AmtPtp8DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtp8ReportDescriptor)    // bDescriptorLength
	},
};

HID_DESCRIPTOR AmtPtpMt2DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                               // bDescriptorType
		sizeof(AmtPtpMt2ReportDescriptor)    // bDescriptorLength
	},
};

#endif
