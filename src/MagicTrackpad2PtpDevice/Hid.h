// Hid.h: Device-related HID definitions
#pragma once

#include "AppleDefinition.h"
#include <hidport.h>

//
// This is the default report descriptor for the Hid device provided
// by the mini driver in response to IOCTL_HID_GET_REPORT_DESCRIPTOR.
// 

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

#define DEVICE_VERSION 0x01

#define REPORTID_STANDARDMOUSE 0x02
#define REPORTID_MULTITOUCH 0x03
#define REPORTID_FEATURE 0x04
#define REPORTID_PTPHQA 0x05
#define REPORTID_FUNCSWITCH 0x06
#define REPORTID_MAX_COUNT 0x07

#define BUTTON_SWITCH 0x57
#define SURFACE_SWITCH 0x58

#define USAGE_PAGE 0x05
#define USAGE_PAGE_1 0x06
#define USAGE      0x09
#define USAGE_MINIMUM 0x19
#define USAGE_MAXIMUM 0x29
#define LOGICAL_MINIMUM 0x15
#define LOGICAL_MAXIMUM 0x25
#define LOGICAL_MAXIMUM_2 0x26
#define LOGICAL_MAXIMUM_3 0x27
#define PHYSICAL_MINIMUM 0x35
#define PHYSICAL_MAXIMUM 0x45
#define PHYSICAL_MAXIMUM_2 0x46
#define PHYSICAL_MAXIMUM_3 0x47
#define UNIT_EXPONENT 0x55
#define UNIT 0x65
#define UNIT_2 0x66

#define REPORT_ID       0x85
#define REPORT_COUNT    0x95
#define REPORT_COUNT_2	0x96
#define REPORT_SIZE     0x75
#define INPUT           0x81
#define FEATURE         0xb1

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

#define AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_1 \
	BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
		LOGICAL_MAXIMUM, 0x01, /* Logical Maximun: 1 */ \
		USAGE, 0x47, /* Usage: Confidence */ \
		USAGE, 0x42, /* Usage: Tip switch */ \
		REPORT_COUNT, 0x02, /* Report Count: 2 */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x01, /* Report Count: 1 */ \
		REPORT_SIZE, 0x03, /* Report Size: 3 */ \
		LOGICAL_MAXIMUM, 0x03, /* Logical Maximun: 3 */ \
		USAGE, 0x51, /* Usage: Contract Identifier */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		REPORT_COUNT, 0x03, /* Report Count: 3 */ \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		USAGE_PAGE, 0x01, /* Usage Page: Generic Desktop */ \
		LOGICAL_MAXIMUM_2, 0xa4, 0x07, /* Logical Maximum: 1956 */ \
		REPORT_SIZE, 0x10, /* Report Size: 0x10 */ \
		UNIT_EXPONENT, 0x0e, /* Unit exponent: -2 */ \
		UNIT, 0x11, /* Unit: SI Length (cm) */ \
		USAGE, 0x30, /* Usage: X */ \
		PHYSICAL_MAXIMUM_2, 0xd2, 0x03, /* Physical Maximum: 978 */ \
		REPORT_COUNT, 0x01, /* Report count: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM_2, 0xf2, 0x01, /* Physical Maximum: 498 */ \
		LOGICAL_MAXIMUM_2, 0xe5, 0x03, /* Logical Maximum: 997 */ \
		USAGE, 0x31, /* Usage: Y */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM, 0x00, /* Physical Maximum: 0 */ \
		UNIT_EXPONENT, 0x00, /* Unit exponent: 0 */ \
		UNIT, 0x00, /* Unit: None */ \
	END_COLLECTION /* End Collection */ \

#define AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_2 \
	BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
		LOGICAL_MAXIMUM, 0x01, /* Logical Maximun: 1 */ \
		USAGE, 0x47, /* Usage: Confidence */ \
		USAGE, 0x42, /* Usage: Tip switch */ \
		REPORT_COUNT, 0x02, /* Report Count: 2 */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x01, /* Report Count: 1 */ \
		REPORT_SIZE, 0x03, /* Report Size: 3 */ \
		LOGICAL_MAXIMUM, 0x03, /* Logical Maximun: 3 */ \
		USAGE, 0x51, /* Usage: Contract Identifier */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		REPORT_COUNT, 0x03, /* Report Count: 3 */ \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		USAGE_PAGE, 0x01, /* Usage Page: Generic Desktop */ \
		LOGICAL_MAXIMUM_2, 0xa4, 0x07, /* Logical Maximum: 1956 */ \
		REPORT_SIZE, 0x10, /* Report Size: 0x10 */ \
		UNIT_EXPONENT, 0x0e, /* Unit exponent: -2 */ \
		UNIT, 0x11, /* Unit: SI Length (cm) */ \
		USAGE, 0x30, /* Usage: X */ \
		PHYSICAL_MAXIMUM_2, 0xd2, 0x03, /* Physical Maximum: 978 */ \
		REPORT_COUNT, 0x01, /* Report count: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM_2, 0xf2, 0x01, /* Physical Maximum: 498 */ \
		LOGICAL_MAXIMUM_2, 0xe5, 0x03, /* Logical Maximum: 997 */ \
		USAGE, 0x31, /* Usage: Y */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
	END_COLLECTION /* End Collection */ \

#define AAPL_MAGIC_TRACKPAD2_PTP_TLC \
	USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
	USAGE, 0x05, /* Usage: Touch Pad */ \
	BEGIN_COLLECTION, 0x01, /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_MULTITOUCH, /* Report ID: Multi-touch */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_1, /* 1 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_1, /* 2 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_2, /* 3 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_1, /* 4 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION_2, /* 5 */ \
				USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		UNIT_EXPONENT, 0x0c, /* Unit exponent: -4 */ \
		UNIT_2, 0x01, 0x10, /* Time: Second */ \
		PHYSICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		LOGICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		USAGE, 0x56, /* Usage: Scan Time */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE, 0x54, /* Usage: Contact Time */ \
		LOGICAL_MAXIMUM, 0x7f, \
		REPORT_SIZE, 0x08, \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE_PAGE, 0x09, /* Usage Page: Button */ \
		USAGE, 0x01, /* Button 1 */ \
		LOGICAL_MAXIMUM, 0x01, \
		REPORT_SIZE, 0x01, \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x07, \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		REPORT_ID, 0x08, \
		USAGE, 0x60, \
		REPORT_COUNT, 0x01, \
		LOGICAL_MAXIMUM, 0x01, \
		FEATURE, 0x02, /* Feature: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x07, \
		FEATURE, 0x03, /* Feature: (Const, Var, Abs) */ \
		REPORT_ID, REPORTID_MAX_COUNT, \
		USAGE, 0x55, \
		USAGE, 0x59, \
		REPORT_SIZE, 0x04, \
		REPORT_COUNT, 0x02, \
		LOGICAL_MAXIMUM, 0x15, \
		FEATURE, 0x02, \
		USAGE_PAGE_1, 0x00, 0xff, \
		USAGE, 0xc6, \
		REPORT_ID, REPORTID_PTPHQA, \
		LOGICAL_MINIMUM, 0x00, \
		LOGICAL_MAXIMUM, 0x08, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT, 0x01, \
		FEATURE, 0x02, \
		USAGE, 0xc7, \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT, 0x20, \
		FEATURE, 0x02, \
	END_COLLECTION /* End Collection */

typedef struct _HID_AAPL_MOUSE_REPORT {
	struct
	{
		UCHAR  bButtons;
		UCHAR  wXData;
		UCHAR  wYData;
		UINT   Padding;
	} InputReport;
} HID_AAPL_MOUSE_REPORT, *PHID_AAPL_MOUSE_REPORT;

typedef struct _HID_INPUT_REPORT
{
	UCHAR ReportID;
	union
	{
		HID_AAPL_MOUSE_REPORT MouseReport;
	};
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;
