// Hid.h: Device-related HID definitions
#pragma once

#include <hidport.h>
#include "AppleDefinition.h"
#include "Types.h"

#define DEVICE_VERSION 0x01

#define REPORTID_STANDARDMOUSE 0x02
#define REPORTID_MULTITOUCH 0x03
#define REPORTID_FEATURE 0x04
#define REPORTID_PTPHQA 0x05
#define REPORTID_FUNCSWITCH 0x06

#define BUTTON_SWITCH 0x57
#define SURFACE_SWITCH 0x58

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
			REPORT_ID, REPORTID_STANDARDMOUSE, /* REPORT_ID: 2 */ \
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

#define AAPL_PTP_CONFIGURATION_TLC \
	USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
	USAGE, 0x0e, /* Usage: Configuration */ \
	BEGIN_COLLECTION, 0x01, /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_FEATURE, /* Report ID: Feature */ \
		USAGE, 0x22, /* Usage: Finger */ \
		BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
			USAGE, 0x52, /* Usage: Input Mode */ \
			LOGICAL_MINIMUM, 0x00, /* Logical Minumum: 0 finger */ \
			LOGICAL_MAXIMUM, MAX_FINGERS, /* Logical Maximum: MAX_TOUCH_COUNT fingers */ \
			REPORT_SIZE, 0x08, /* Report Size: 0x08 */ \
			REPORT_COUNT, 0x01, /* Report Count: 0x01 */ \
			INPUT, 0x02, /* Input: (Data, Var, Avs) */ \
		END_COLLECTION, /* End Collection */ \
		BEGIN_COLLECTION, 0x00, /* Begin Collection: Physical */ \
			REPORT_ID, REPORTID_FUNCSWITCH, /* Report ID: Function Switch */ \
			USAGE, BUTTON_SWITCH, /* Usage: Button Switch */ \
			USAGE, SURFACE_SWITCH, /* Usage: Surface Switch */ \
			REPORT_SIZE, 0x01, /* Report Size: 0x01 */ \
			REPORT_COUNT, 0x02, /* Report Count: 0x02 */ \
			LOGICAL_MAXIMUM, 0x01, /* Logical Maximum: 0x01 */ \
			INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
			REPORT_COUNT, 0x06, /* Report Count: 0x06 */ \
			INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		END_COLLECTION, /* End Collection */ \
	END_COLLECTION /* End Collection */

HID_REPORT_DESCRIPTOR AAPLMagicTrackpad2ReportDescriptor[] = {
	AAPL_MAGIC_TRACKPAD2_MOUSE_TLC,
	AAPL_PTP_CONFIGURATION_TLC
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
