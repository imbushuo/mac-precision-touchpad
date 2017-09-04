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
#define REPORTID_REPORTMODE 0x04
#define REPORTID_PTPHQA 0x05
#define REPORTID_FUNCSWITCH 0x06
#define REPORTID_DEVICE_CAPS 0x07

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
		REPORT_ID, REPORTID_REPORTMODE, /* Report ID: Mode Selection */ \
		USAGE, 0x22, /* Usage: Finger */ \
		BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
			USAGE, 0x52, /* Usage: Input Mode */ \
			LOGICAL_MINIMUM, 0x00, /* Logical Minumum: 0 finger */ \
			LOGICAL_MAXIMUM, MAX_FINGERS, /* Logical Maximum: MAX_TOUCH_COUNT fingers */ \
			REPORT_SIZE, 0x08, /* Report Size: 0x08 */ \
			REPORT_COUNT, 0x01, /* Report Count: 0x01 */ \
			FEATURE, 0x02, /* Feature: (Data, Var, Avs) */ \
		END_COLLECTION, /* End Collection */ \
		BEGIN_COLLECTION, 0x00, /* Begin Collection: Physical */ \
			REPORT_ID, REPORTID_FUNCSWITCH, /* Report ID: Function Switch */ \
			USAGE, BUTTON_SWITCH, /* Usage: Button Switch */ \
			USAGE, SURFACE_SWITCH, /* Usage: Surface Switch */ \
			REPORT_SIZE, 0x08, /* Report Size: 0x08 */ \
			REPORT_COUNT, 0x02, /* Report Count: 0x02 */ \
			LOGICAL_MAXIMUM, 0x01, /* Logical Maximum: 0x01 */ \
			FEATURE, 0x02, /* Feature: (Data, Var, Abs) */ \
		END_COLLECTION, /* End Collection */ \
	END_COLLECTION /* End Collection */

#define AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION \
	BEGIN_COLLECTION, 0x02, /* Begin Collection: Logical */ \
		/* Begin a byte */ \
		LOGICAL_MAXIMUM, 0x01, /* Logical Maximum: 1 */ \
		USAGE, 0x47, /* Usage: Confidence */ \
		USAGE, 0x42, /* Usage: Tip switch */ \
		REPORT_COUNT, 0x02, /* Report Count: 2 */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_COUNT, 0x06, /* Report Count: 6 */ \
		REPORT_SIZE, 0x01, /* Report Size: 1*/ \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		/* End of a byte */ \
		/* Begin a byte */ \
		REPORT_COUNT, 0x01, /* Report Count: 1 */ \
		REPORT_SIZE, 0x03, /* Report Size: 3 */ \
		LOGICAL_MAXIMUM, 0x03, /* Logical Maximum: 3 */ \
		USAGE, 0x51, /* Usage: Contract Identifier */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		REPORT_SIZE, 0x01, /* Report Size: 1 */ \
		REPORT_COUNT, 0x05, /* Report Count: 3 */ \
		INPUT, 0x03, /* Input: (Const, Var, Abs) */ \
		/* End of a byte */ \
		/* Begin of 6 bytes */ \
		/* Size is hard-coded at this moment */ \
		USAGE_PAGE, 0x01, /* Usage Page: Generic Desktop */ \
		REPORT_SIZE, 0x10, /* Report Size: 0x10 (2 bytes) */ \
		UNIT_EXPONENT, 0x0e, /* Unit exponent: -2 */ \
		UNIT, 0x11, /* Unit: SI Length (cm) */ \
		USAGE, 0x30, /* Usage: X */ \
		LOGICAL_MAXIMUM_2, 0xbc, 0x10, /* Logical Maximum: 7612 (See defintion) */ \
		PHYSICAL_MAXIMUM_2, 0x40, 0x06, /* Physical Maximum: 1600 (See Apple Spec) */ \
		REPORT_COUNT, 0x01, /* Report count: 1 */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		PHYSICAL_MAXIMUM_2, 0x7d, 0x04, /* Physical Maximum: 1149 (See Apple Spec) */ \
		LOGICAL_MAXIMUM_2, 0xc9, 0x13, /* Logical Maximum: 5065 (See definition) */ \
		USAGE, 0x31, /* Usage: Y */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		LOGICAL_MAXIMUM_2, 0x2c, 0x01, /* Logical Maximum: 300 */ \
		USAGE, 0x30, /* Usage: Tip Pressure */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		/* End of 6 bytes */ \
	END_COLLECTION /* End Collection */ \

#define AAPL_MAGIC_TRACKPAD2_PTP_TLC \
	USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
	USAGE, 0x05, /* Usage: Touch Pad */ \
	BEGIN_COLLECTION, 0x01, /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_MULTITOUCH, /* Report ID: Multi-touch */ \
		USAGE, 0x22, /* Usage: Finger */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION, /* 1 */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION, /* 2 */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION, /* 3 */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION, /* 4 */ \
		AAPL_MAGIC_TRACKPAD2_PTP_FINGER_COLLECTION, /* 5 */ \
		USAGE_PAGE, 0x0d, /* Usage Page: Digitizer */ \
		UNIT_EXPONENT, 0x0c, /* Unit exponent: -4 */ \
		UNIT_2, 0x01, 0x10, /* Time: Second */ \
		PHYSICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		LOGICAL_MAXIMUM_3, 0xff, 0xff, 0x00, 0x00, \
		USAGE, 0x56, /* Usage: Scan Time */ \
		INPUT, 0x02, /* Input: (Data, Var, Abs) */ \
		USAGE, 0x54, /* Usage: Contact Count */ \
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
		REPORT_ID, REPORTID_DEVICE_CAPS, \
		USAGE, 0x55, /* Usage: Maximum Contacts */ \
		USAGE, 0x59, /* Usage: Touchpad Button Type*/ \
		LOGICAL_MINIMUM, 0x00, \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT, 0x02, \
		FEATURE, 0x02, \
		USAGE_PAGE_1, 0x00, 0xff, \
		REPORT_ID, REPORTID_PTPHQA, \
		USAGE, 0xc5, \
		LOGICAL_MINIMUM, 0x00, \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, \
		REPORT_SIZE, 0x08, \
		REPORT_COUNT_2, 0x00, 0x01, \
		FEATURE, 0x02, \
	END_COLLECTION /* End Collection */

#define DEFAULT_PTP_HQA_BLOB \
	0xfc, 0x28, 0xfe, 0x84, 0x40, 0xcb, 0x9a, 0x87, \
	0x0d, 0xbe, 0x57, 0x3c, 0xb6, 0x70, 0x09, 0x88, \
	0x07, 0x97, 0x2d, 0x2b, 0xe3, 0x38, 0x34, 0xb6, \
	0x6c, 0xed, 0xb0, 0xf7, 0xe5, 0x9c, 0xf6, 0xc2, \
	0x2e, 0x84, 0x1b, 0xe8, 0xb4, 0x51, 0x78, 0x43, \
	0x1f, 0x28, 0x4b, 0x7c, 0x2d, 0x53, 0xaf, 0xfc, \
	0x47, 0x70, 0x1b, 0x59, 0x6f, 0x74, 0x43, 0xc4, \
	0xf3, 0x47, 0x18, 0x53, 0x1a, 0xa2, 0xa1, 0x71, \
	0xc7, 0x95, 0x0e, 0x31, 0x55, 0x21, 0xd3, 0xb5, \
	0x1e, 0xe9, 0x0c, 0xba, 0xec, 0xb8, 0x89, 0x19, \
	0x3e, 0xb3, 0xaf, 0x75, 0x81, 0x9d, 0x53, 0xb9, \
	0x41, 0x57, 0xf4, 0x6d, 0x39, 0x25, 0x29, 0x7c, \
	0x87, 0xd9, 0xb4, 0x98, 0x45, 0x7d, 0xa7, 0x26, \
	0x9c, 0x65, 0x3b, 0x85, 0x68, 0x89, 0xd7, 0x3b, \
	0xbd, 0xff, 0x14, 0x67, 0xf2, 0x2b, 0xf0, 0x2a, \
	0x41, 0x54, 0xf0, 0xfd, 0x2c, 0x66, 0x7c, 0xf8, \
	0xc0, 0x8f, 0x33, 0x13, 0x03, 0xf1, 0xd3, 0xc1, \
	0x0b, 0x89, 0xd9, 0x1b, 0x62, 0xcd, 0x51, 0xb7, \
	0x80, 0xb8, 0xaf, 0x3a, 0x10, 0xc1, 0x8a, 0x5b, \
	0xe8, 0x8a, 0x56, 0xf0, 0x8c, 0xaa, 0xfa, 0x35, \
	0xe9, 0x42, 0xc4, 0xd8, 0x55, 0xc3, 0x38, 0xcc, \
	0x2b, 0x53, 0x5c, 0x69, 0x52, 0xd5, 0xc8, 0x73, \
	0x02, 0x38, 0x7c, 0x73, 0xb6, 0x41, 0xe7, 0xff, \
	0x05, 0xd8, 0x2b, 0x79, 0x9a, 0xe2, 0x34, 0x60, \
	0x8f, 0xa3, 0x32, 0x1f, 0x09, 0x78, 0x62, 0xbc, \
	0x80, 0xe3, 0x0f, 0xbd, 0x65, 0x20, 0x08, 0x13, \
	0xc1, 0xe2, 0xee, 0x53, 0x2d, 0x86, 0x7e, 0xa7, \
	0x5a, 0xc5, 0xd3, 0x7d, 0x98, 0xbe, 0x31, 0x48, \
	0x1f, 0xfb, 0xda, 0xaf, 0xa2, 0xa8, 0x6a, 0x89, \
	0xd6, 0xbf, 0xf2, 0xd3, 0x32, 0x2a, 0x9a, 0xe4, \
	0xcf, 0x17, 0xb7, 0xb8, 0xf4, 0xe1, 0x33, 0x08, \
	0x24, 0x8b, 0xc4, 0x43, 0xa5, 0xe5, 0x24, 0xc2

#define PTP_MAX_CONTACT_POINTS 5
#define PTP_BUTTON_TYPE_CLICK_PAD 0
#define PTP_BUTTON_TYPE_PRESSURE_PAD 1

#define PTP_COLLECTION_MOUSE 0
#define PTP_COLLECTION_WINDOWS 3

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
	HID_AAPL_MOUSE_REPORT MouseReport;
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

typedef struct _PTP_DEVICE_CAPS_FEATURE_REPORT
{
	UCHAR ReportID;
	UCHAR MaximumContactPoints;
	UCHAR ButtonType;
} PTP_DEVICE_CAPS_FEATURE_REPORT, *PPTP_DEVICE_CAPS_FEATURE_REPORT;

typedef struct _PTP_DEVICE_HQA_CERTIFICATION_REPORT
{
	UCHAR ReportID;
	UCHAR CertificationBlob[256];
} PTP_DEVICE_HQA_CERTIFICATION_REPORT, *PPTP_DEVICE_HQA_CERTIFICATION_REPORT;

typedef struct _PTP_DEVICE_INPUT_MODE_REPORT
{
	UCHAR ReportID;
	UCHAR Mode;
} PTP_DEVICE_INPUT_MODE_REPORT, *PPTP_DEVICE_INPUT_MODE_REPORT;

typedef struct _PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT
{
	UCHAR ReportID;
	UCHAR ButtonReport;
	UCHAR SurfaceReport;
} PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT, *PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT;