// HidDevice.h: devicei-specific HID structures
#pragma once


/* button data structure */
typedef struct _TRACKPAD_BUTTON_DATA {
	UCHAR unknown1;			/* constant */
	UCHAR button;			/* left button */
	UCHAR rel_x;			/* relative x coordinate */
	UCHAR rel_y;			/* relative y coordinate */
} TRACKPAD_BUTTON_DATA, *PTRACKPAD_BUTTON_DATA;

#include <pshpack2.h>
/* trackpad finger structure, le16-aligned */
typedef struct _TRACKPAD_FINGER {
	USHORT origin;		/* zero when switching track finger */
	USHORT abs_x;		/* absolute x coodinate */
	USHORT abs_y;		/* absolute y coodinate */
	USHORT rel_x;		/* relative x coodinate */
	USHORT rel_y;		/* relative y coodinate */
	USHORT tool_major;	/* tool area, major axis */
	USHORT tool_minor;	/* tool area, minor axis */
	USHORT orientation;	/* 16384 when point, else 15 bit angle */
	USHORT touch_major;	/* touch area, major axis */
	USHORT touch_minor;	/* touch area, minor axis */
	USHORT unused[2];	/* zeros */
	USHORT pressure;	/* pressure on forcetouch touchpad */
	USHORT multi;		/* one finger: varies, more fingers: constant */
} TRACKPAD_FINGER, *PTRACKPAD_FINGER;

#include <poppack.h>

#pragma pack( push, 1 )
#pragma warning( push )
#pragma warning( disable : 4200 )

/* Trackpad finger structure for type5 (magic trackpad) */
typedef struct _TRACKPAD_FINGER_TYPE5
{
	UINT32 AbsoluteX : 13;		/* absolute x coordinate */
	UINT32 AbsoluteY : 13;		/* absolute y coordinate */
	UINT32 Finger : 3;			/* finger type */
	UINT32 State : 3;			/* finger State */
	UCHAR TouchMajor;			/* touch area, major axis */
	UCHAR TouchMinor;			/* touch area, minor axis */
	UCHAR Size;					/* tool area, size */
	UCHAR Pressure;				/* pressure on forcetouch touchpad */
	UCHAR Id : 4;				/* slot id */
	UCHAR _ : 1;
	UCHAR Orientation : 3;		/* contact angle */
} TRACKPAD_FINGER_TYPE5;

/* Appended Mouse report on front of MT2 USB reports */
typedef struct _MOUSE_REPORT
{
	UCHAR ReportId;
	UCHAR Button;
	UCHAR X;
	UCHAR Y;
	UCHAR _[4];
} MOUSE_REPORT;

/* Multitouch report from MT2 */
typedef struct _TRACKPAD_REPORT_TYPE5
{
	UCHAR ReportId;
	UINT8 Button : 1;
	UINT8 _ : 2;
	UINT8 TimestampLow : 5;
	UINT16 TimestampHigh;
	TRACKPAD_FINGER_TYPE5 Fingers[];
} TRACKPAD_REPORT_TYPE5;

static_assert(sizeof(TRACKPAD_FINGER_TYPE5) == 9, "Unexpected MAGIC_TRACKPAD_INPUT_REPORT_FINGER size");
static_assert(sizeof(TRACKPAD_REPORT_TYPE5) == 4, "Unexpected MAGIC_TRACKPAD_INPUT_REPORT_FINGER size");

#pragma warning( pop )
#pragma pack( pop )
