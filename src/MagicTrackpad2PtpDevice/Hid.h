// Hid.h: Device-related HID definitions
#pragma once

#include <hidport.h>
#include "Types.h"

#define DEVICE_VERSION 0x01

#define REPORTID_MTOUCH 0x01
#define REPORTID_FEATURE 0x02
#define REPORTID_MOUSE 0x03
#define REPORTID_PTPHQA 0x04

#define USAGE_PAGE 0x05
#define USAGE      0x09
#define USAGE_MINIMUM 0x19
#define USAGE_MAXIMUM 0x29
#define LOGICAL_MINIMUM 0x15
#define LOGICAL_MAXIMUM 0x25

#define REPORT_ID       0x85
#define REPORT_COUNT    0x95
#define REPORT_SIZE     0x75
#define INPUT           0x81

#define BEGIN_COLLECTION 0xa1
#define END_COLLECTION   0xc0

#define AAPL_MAGIC_TRACKPAD2_MOUSE_TLC \
	USAGE_PAGE, 0x01, /* USAGE_PAGE: Generic Desktop */ \
	USAGE, 0x02, /* USAGE: Mouse */ \
	BEGIN_COLLECTION, 0x01, /* BEGIN COLLECTION: Application */ \
		USAGE, 0x01, /* USAGE: Generic desktop controls (Mouse) */ \
		BEGIN_COLLECTION, 0x00, /* BEGIN COLLECTION: Physical */ \
			USAGE_PAGE, 0x09, /* USAGE_PAGE: Button */ \
			USAGE_MINIMUM, 0x01, /* USAGE_MINIMUM: (Button 1) */ \
			USAGE_MAXIMUM, 0x03, /* USAGE_MAXIMUM: (Button 3) */ \
			LOGICAL_MINIMUM, 0x00, /* LOGICAL_MINIMUM: 0 */ \
			LOGICAL_MAXIMUM, 0x01, /* LOGICAL_MAXIMUM: 1 */ \
			REPORT_ID, 0x02, /* REPORT_ID: 2 */ \
			REPORT_COUNT, 0x03, /* REPORT_COUNT: 3 */ \
			REPORT_SIZE, 0x01, /* REPORT_SIZE: 1 */ \
			INPUT, 0x02, /* INPUT: (Data, Var, Abs) */ \
			REPORT_COUNT, 0x01, /* REPORT_COUNT: 1 */ \
			REPORT_SIZE, 0x05, /* REPORT_SIZE: 5 */ \
			INPUT, 0x01, /* INPUT: (Const, Arr, Abs) */ \
			USAGE_PAGE, 0x01, /* USAGE_PAGE: Generic desktop controls (Mouse) */ \
			USAGE, 0x30, /* USAGE: Reserved */ \
			USAGE, 0x31, /* USAGE: Reserved */ \
			LOGICAL_MINIMUM, 0x81, /* LOGICAL_MINIMUM: -129 */ \
			LOGICAL_MAXIMUM, 0x7F, /* LOGICAL_MAXIMUM: 127 */ \
			REPORT_SIZE, 0x08, /* REPORT_SIZE: 8 */ \
			REPORT_COUNT, 0x02, /* REPORT_COUNT: 2 */ \
			INPUT, 0x06, /* (INPUT: (Data, Var, Rel) */ \
			REPORT_COUNT, 0x04, /* REPORT_COUNT: 4 */ \
			REPORT_SIZE, 0x08, /* REPORT_SIZE: 8 */ \
			INPUT, 0x01, /* INPUT: (Const, Arr, Abs) */ \
		END_COLLECTION,       /* END COLLECTION */ \
	END_COLLECTION		/* END COLLECTION */

HID_REPORT_DESCRIPTOR AAPLMagicTrackpad2ReportDescriptor[] = {
	// Only report Mouse at this moment
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
