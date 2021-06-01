// StaticHidRegistry.h: Static HID collection information
#pragma once

#include <hidport.h>

#include <HidCommon.h>
#include <Metadata/WindowsHID.h>
#include <Metadata/MagicTrackpad2.h>

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

#ifndef _STATIC_HID_REGISTRY_H_
#define _STATIC_HID_REGISTRY_H_

static HID_REPORT_DESCRIPTOR PtpReportDescriptorMagicTrackpad2[] = {
	AAPL_MAGIC_TRACKPAD2_PTP_TLC,
	AAPL_PTP_WINDOWS_CONFIGURATION_TLC,
};

static HID_DESCRIPTOR PtpDefaultHidDescriptorMagicTrackpad2 = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,											// bDescriptorType
		sizeof(PtpReportDescriptorMagicTrackpad2),		// bDescriptorLength
	},
};

#endif
