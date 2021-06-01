// Hidminiport.h: HID miniport communication structures
#pragma once

#include <pshpack1.h>

// PTP device capabilites Feature Report
typedef struct _PTP_DEVICE_CAPS_FEATURE_REPORT {
	UCHAR ReportID;
	UCHAR MaximumContactPoints;
	UCHAR ButtonType;
} PTP_DEVICE_CAPS_FEATURE_REPORT, * PPTP_DEVICE_CAPS_FEATURE_REPORT;

// PTP device certification Feature Report
typedef struct _PTP_DEVICE_HQA_CERTIFICATION_REPORT {
	UCHAR ReportID;
	UCHAR CertificationBlob[256];
} PTP_DEVICE_HQA_CERTIFICATION_REPORT, * PPTP_DEVICE_HQA_CERTIFICATION_REPORT;

// PTP input mode Feature Report
typedef struct _PTP_DEVICE_INPUT_MODE_REPORT {
	UCHAR ReportID;
	UCHAR Mode;
} PTP_DEVICE_INPUT_MODE_REPORT, * PPTP_DEVICE_INPUT_MODE_REPORT;

// PTP input selection Feature Report
typedef struct _PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT {
	UCHAR ReportID;
	UCHAR ButtonReport : 1;
	UCHAR SurfaceReport : 1;
	UCHAR Padding : 6;
} PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT, * PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT;

// PTP single finger
typedef struct _PTP_CONTACT {
	UCHAR		Confidence : 1;
	UCHAR		TipSwitch : 1;
	UCHAR		Padding : 6;
	ULONG		ContactID;
	USHORT		X;
	USHORT		Y;
} PTP_CONTACT, * PPTP_CONTACT;

// PTP single scan frame Input Report
typedef struct _PTP_REPORT {
	UCHAR       ReportID;
	PTP_CONTACT Contacts[5];
	USHORT      ScanTime;
	UCHAR       ContactCount;
	UCHAR       IsButtonClicked;
} PTP_REPORT, * PPTP_REPORT;

#include <poppack.h>

// HID routines
NTSTATUS
PtpFilterGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

NTSTATUS
PtpFilterGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

NTSTATUS
PtpFilterGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

NTSTATUS
PtpFilterGetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request,
	_Out_ BOOLEAN* Pending
);

NTSTATUS
PtpFilterGetHidFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);

NTSTATUS
PtpFilterSetHidFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
);
