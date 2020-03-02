// Hid.h: Device-related HID definitions
#pragma once

#include <hidport.h>

#include <AppleDefinition.h>
#include <HidCommon.h>

// Device family metadata
#include "DeviceFamily/Wellspring3.h"
#include "DeviceFamily/Wellspring5.h"
#include "DeviceFamily/Wellspring6.h"
#include "DeviceFamily/Wellspring7A.h"
#include "DeviceFamily/Wellspring8.h"
#include "DeviceFamily/WellspringMt2.h"

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

#define DEVICE_VERSION 0x01

#define AAPL_PTP_USERMODE_CONFIGURATION_APP_TLC \
	USAGE_PAGE_1, 0x00, 0xff, /* Usage Page: Vendor defined */ \
	USAGE, 0x01, /* Usage: Vendor Usage 0x01 */ \
	BEGIN_COLLECTION, 0x01, /* Begin Collection: Application */ \
		REPORT_ID, REPORTID_UMAPP_CONF, /* Report ID: User-mode Application configuration */ \
		USAGE, 0x01, /* Usage: Vendor Usage 0x01 */ \
		LOGICAL_MINIMUM, 0x00, /* Logical Minimum 0 */ \
		LOGICAL_MAXIMUM_2, 0xff, 0x00, /* Logical Maximum 255 */ \
		REPORT_SIZE, 0x08, /* Report Size: 8 */ \
		REPORT_COUNT, 0x03, /* Report Count: 3 */ \
		FEATURE, 0x02, /* Feature: (Data, Var, Abs) */ \
	END_COLLECTION

#define AAPL_PTP_WINDOWS_CONFIGURATION_TLC \
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
			FEATURE, 0x02, /* Feature: (Data, Var, Abs) */ \
		END_COLLECTION, /* End Collection */ \
		BEGIN_COLLECTION, 0x00, /* Begin Collection: Physical */ \
			REPORT_ID, REPORTID_FUNCSWITCH, /* Report ID: Function Switch */ \
			USAGE, BUTTON_SWITCH, /* Usage: Button Switch */ \
			USAGE, SURFACE_SWITCH, /* Usage: Surface Switch */ \
			REPORT_SIZE, 0x01, /* Report Size: 0x01 */ \
			REPORT_COUNT, 0x02, /* Report Count: 0x02 */ \
			LOGICAL_MAXIMUM, 0x01, /* Logical Maximum: 0x01 */ \
			FEATURE, 0x02, /* Feature: (Data, Var, Abs) */ \
			REPORT_COUNT, 0x06, /* Report Count: 0x06 */ \
			FEATURE, 0x03, /* Feature: (Const, Var, Abs) */ \
		END_COLLECTION, /* End Collection */ \
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

#define PTP_CONTACT_CONFIDENCE_BIT   1
#define PTP_CONTACT_TIPSWITCH_BIT    2

typedef struct _HID_AAPL_MOUSE_REPORT {
	struct {
		UCHAR  bButtons;
		UCHAR  wXData;
		UCHAR  wYData;
		UINT   Padding;
	} InputReport;
} HID_AAPL_MOUSE_REPORT, *PHID_AAPL_MOUSE_REPORT;

typedef struct _HID_INPUT_REPORT {
	UCHAR ReportID;
	HID_AAPL_MOUSE_REPORT MouseReport;
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

typedef struct _PTP_DEVICE_CAPS_FEATURE_REPORT {
	UCHAR ReportID;
	UCHAR MaximumContactPoints;
	UCHAR ButtonType;
} PTP_DEVICE_CAPS_FEATURE_REPORT, *PPTP_DEVICE_CAPS_FEATURE_REPORT;

typedef struct _PTP_DEVICE_HQA_CERTIFICATION_REPORT {
	UCHAR ReportID;
	UCHAR CertificationBlob[256];
} PTP_DEVICE_HQA_CERTIFICATION_REPORT, *PPTP_DEVICE_HQA_CERTIFICATION_REPORT;

typedef struct _PTP_DEVICE_INPUT_MODE_REPORT {
	UCHAR ReportID;
	UCHAR Mode;
} PTP_DEVICE_INPUT_MODE_REPORT, *PPTP_DEVICE_INPUT_MODE_REPORT;

#pragma pack(1)
typedef struct _PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT {
	UCHAR ReportID;
	UCHAR ButtonReport : 1;
	UCHAR SurfaceReport : 1;
	UCHAR Padding : 6;
} PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT, *PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT;
#pragma pack()

#pragma pack(1)
typedef struct _PTP_CONTACT {
	UCHAR		Confidence : 1;
	UCHAR		TipSwitch  : 1;
	UCHAR		Padding    : 6;
	ULONG		ContactID;
	USHORT		X;
	USHORT		Y;
} PTP_CONTACT, *PPTP_CONTACT;
#pragma pack()

typedef struct _PTP_REPORT {
	UCHAR       ReportID;
	PTP_CONTACT Contacts[5];
	USHORT      ScanTime;
	UCHAR       ContactCount;
	UCHAR       IsButtonClicked;
} PTP_REPORT, *PPTP_REPORT;

typedef struct _PTP_USERMODEAPP_CONF_REPORT {
	UCHAR		ReportID;
	UCHAR		PressureQualificationLevel;
	UCHAR		SingleContactSizeQualificationLevel;
	UCHAR		MultipleContactSizeQualificationLevel;
} PTP_USERMODEAPP_CONF_REPORT, *PPTP_USERMODEAPP_CONF_REPORT;
