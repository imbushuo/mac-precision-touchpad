// HidDevice.h: devicei-specific HID structures
#pragma once

/* Trackpad finger data offsets, le16-aligned */
#define HOFFSET_TYPE_USB_1		(13 * sizeof(USHORT))
#define HOFFSET_TYPE_USB_2		(15 * sizeof(USHORT))
#define HOFFSET_TYPE_USB_3		(19 * sizeof(USHORT))
#define HOFFSET_TYPE_USB_4		(23 * sizeof(USHORT))
#define HOFFSET_TYPE_USB_5		( 6 * sizeof(USHORT))
#define HOFFSET_TYPE_BTH_5		( 2 * sizeof(USHORT))

/* Trackpad button data offsets */
#define BOFFSET_TYPE1		0
#define BOFFSET_TYPE2		15
#define BOFFSET_TYPE3		23
#define BOFFSET_TYPE4		31
#define BOFFSET_TYPE5		1

/* Trackpad finger data block size */
#define FSIZE_TYPE1		(14 * sizeof(USHORT))
#define FSIZE_TYPE2		(14 * sizeof(USHORT))
#define FSIZE_TYPE3		(14 * sizeof(USHORT))
#define FSIZE_TYPE4		(15 * sizeof(USHORT))
#define FSIZE_TYPE5		(9)

/* List of device capability bits */
#define HAS_INTEGRATED_BUTTON	1

/* Offset from header to finger struct */
#define FDELTA_TYPE1		(0 * sizeof(USHORT))
#define FDELTA_TYPE2		(0 * sizeof(USHORT))
#define FDELTA_TYPE3		(0 * sizeof(USHORT))
#define FDELTA_TYPE4		(1 * sizeof(USHORT))
#define FDELTA_TYPE5		(0 * sizeof(USHORT))

/* Trackpad finger data size, empirically at least ten fingers */
#define MAX_FINGERS		16
#define MAX_FINGER_ORIENTATION	16384

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

/* Trackpad finger structure for type5 (magic trackpad), le16-aligned */
typedef struct _TRACKPAD_FINGER_TYPE5
{
	UCHAR AbsoluteX;			/* absolute x coodinate */
	UCHAR AbsoluteXY;			/* absolute x,y coodinate */
	UCHAR AbsoluteY[2];			/* absolute y coodinate */
	UCHAR TouchMajor;			/* touch area, major axis */
	UCHAR TouchMinor;			/* touch area, minor axis */
	UCHAR Size;					/* tool area, size */
	UCHAR Pressure;				/* pressure on forcetouch touchpad */
	union _ORIDENTATION_AND_ORIGIN
	{
		struct _CONTACT_IDENTIFIER
		{
			UCHAR Id : 4;
			UCHAR Orientation : 4;
		} ContactIdentifier;
		UCHAR RawValue;
	} OrientationAndOrigin;
} TRACKPAD_FINGER_TYPE5, *PTRACKPAD_FINGER_TYPE5;
#include <poppack.h>
