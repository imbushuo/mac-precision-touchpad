#pragma once

#define USB_VENDOR_ID_APPLE		0x05ac

/* MacbookAir, aka wellspring */
#define USB_DEVICE_ID_APPLE_WELLSPRING_ANSI	0x0223
#define USB_DEVICE_ID_APPLE_WELLSPRING_ISO	0x0224
#define USB_DEVICE_ID_APPLE_WELLSPRING_JIS	0x0225
/* MacbookProPenryn, aka wellspring2 */
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI	0x0230
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ISO	0x0231
#define USB_DEVICE_ID_APPLE_WELLSPRING2_JIS	0x0232
/* Macbook5,1 (unibody), aka wellspring3 */
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI	0x0236
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ISO	0x0237
#define USB_DEVICE_ID_APPLE_WELLSPRING3_JIS	0x0238
/* MacbookAir3,2 (unibody), aka wellspring5 */
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI	0x023f
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ISO	0x0240
#define USB_DEVICE_ID_APPLE_WELLSPRING4_JIS	0x0241
/* MacbookAir3,1 (unibody), aka wellspring4 */
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI	0x0242
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO	0x0243
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS	0x0244
/* Macbook8 (unibody, March 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI	0x0245
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ISO	0x0246
#define USB_DEVICE_ID_APPLE_WELLSPRING5_JIS	0x0247
/* MacbookAir4,1 (unibody, July 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI	0x0249
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO	0x024a
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS	0x024b
/* MacbookAir4,2 (unibody, July 2011) */
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI	0x024c
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ISO	0x024d
#define USB_DEVICE_ID_APPLE_WELLSPRING6_JIS	0x024e
/* Macbook8,2 (unibody) */
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI	0x0252
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO	0x0253
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS	0x0254
/* MacbookPro10,1 (unibody, June 2012) */
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI	0x0262
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ISO	0x0263
#define USB_DEVICE_ID_APPLE_WELLSPRING7_JIS	0x0264
/* MacbookPro10,2 (unibody, October 2012) */
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI	0x0259
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO	0x025a
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS	0x025b
/* MacbookAir6,2 (unibody, June 2013) */
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI	0x0290
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ISO	0x0291
#define USB_DEVICE_ID_APPLE_WELLSPRING8_JIS	0x0292
/* MacbookPro12,1 (2015) */
#define USB_DEVICE_ID_APPLE_WELLSPRING9_ANSI	0x0272
#define USB_DEVICE_ID_APPLE_WELLSPRING9_ISO	0x0273
#define USB_DEVICE_ID_APPLE_WELLSPRING9_JIS	0x0274
/* MagicTrackpad2 (2015) */
#define USB_DEVICE_ID_APPLE_MAGICTRACKPAD2	0x0265
/* Apple T2 USB trackpad */
#define USB_DEVICE_ID_APPLE_T2 0x027d

/* button data structure */
struct TRACKPAD_BUTTON_DATA {
	UCHAR unknown1;		/* constant */
	UCHAR button;			/* left button */
	UCHAR rel_x;			/* relative x coordinate */
	UCHAR rel_y;			/* relative y coordinate */
};

/* trackpad header types */
enum TRACKPAD_TYPE {
	TYPE1,			/* plain trackpad */
	TYPE2,			/* button integrated in trackpad */
	TYPE3,			/* additional header fields since June 2013 */
	TYPE4,			/* additional header field for pressure data */
	TYPE5			/* format for magic trackpad 2 */
};

/* Trackpad finger data offsets, le16-aligned */
#define HEADER_TYPE1		(13 * sizeof(USHORT))
#define HEADER_TYPE2		(15 * sizeof(USHORT))
#define HEADER_TYPE3		(19 * sizeof(USHORT))
#define HEADER_TYPE4		(23 * sizeof(USHORT))
#define HEADER_TYPE5		( 6 * sizeof(USHORT))

/* Trackpad button data offsets */
#define BUTTON_TYPE1		0
#define BUTTON_TYPE2		15
#define BUTTON_TYPE3		23
#define BUTTON_TYPE4		31
#define BUTTON_TYPE5		1

/* List of device capability bits */
#define HAS_INTEGRATED_BUTTON	1

/* Trackpad finger data block size */
#define FSIZE_TYPE1		(14 * sizeof(USHORT))
#define FSIZE_TYPE2		(14 * sizeof(USHORT))
#define FSIZE_TYPE3		(14 * sizeof(USHORT))
#define FSIZE_TYPE4		(15 * sizeof(USHORT))
#define FSIZE_TYPE5		(9)

/* Offset from header to finger struct */
#define DELTA_TYPE1		(0 * sizeof(USHORT))
#define DELTA_TYPE2		(0 * sizeof(USHORT))
#define DELTA_TYPE3		(0 * sizeof(USHORT))
#define DELTA_TYPE4		(1 * sizeof(USHORT))
#define DELTA_TYPE5		(0 * sizeof(USHORT))

/* USB control message mode switch data */
#define USBMSG_TYPE1	8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE2	8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE3	8, 0x300, 0, 0, 0x1, 0x8
#define USBMSG_TYPE4	2, 0x302, 2, 1, 0x1, 0x0
#define USBMSG_TYPE5	2, 0x302, 1, 1, 0x1, 0x0

/* Wellspring initialization constants */
#define BCM5974_WELLSPRING_MODE_READ_REQUEST_ID		1
#define BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID	9

/* Trackpad finger data size, empirically at least ten fingers */
#define MAX_FINGERS		16
#define MAX_FINGER_ORIENTATION	16384

#define BCM5974_MOUSE_SIZE 8

/* trackpad finger structure, le16-aligned */
__declspec(align(2)) struct TRACKPAD_FINGER {
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
};

/* Trackpad finger structure for type5 (magic trackpad), le16-aligned */
__declspec(align(2)) struct TRACKPAD_FINGER_TYPE5 
{
	UCHAR AbsoluteX;			/* absolute x coodinate */
	UCHAR AbsoluteXY;			/* absolute x,y coodinate */
	UCHAR AbsoluteY[2];			/* absolute y coodinate */
	UCHAR TouchMajor;			/* touch area, major axis */
	UCHAR TouchMinor;			/* touch area, minor axis */
	UCHAR Size;					/* tool area, size */
	UCHAR Pressure;				/* pressure on forcetouch touchpad */
	union 
	{
		struct 
		{
			UCHAR Id : 4;
			UCHAR Orientation : 4;
		} ContactIdentifier;
		UCHAR RawOrientationAndOrigin;
	};
};

/* device-specific parameters */
struct BCM5974_PARAM {
	int snratio;		/* signal-to-noise ratio */
	int min;			/* device minimum reading */
	int max;			/* device maximum reading */
};

/* device-specific configuration */
struct BCM5974_CONFIG {
	int ansi, iso, jis;				/* the product id of this device */
	int caps;						/* device capability bitmask */
	int bt_ep;						/* the endpoint of the button interface */
	int bt_datalen;					/* data length of the button interface */
	int tp_ep;						/* the endpoint of the trackpad interface */
	enum TRACKPAD_TYPE tp_type;		/* type of trackpad interface */
	int tp_header;					/* bytes in header block */
	int tp_datalen;					/* data length of the trackpad interface */
	int tp_button;					/* offset to button data */
	int tp_fsize;					/* bytes in single finger block */
	int tp_delta;					/* offset from header to finger struct */
	int um_size;					/* usb control message length */
	int um_req_val;					/* usb control message value */
	int um_req_idx;					/* usb control message index */
	int um_switch_idx;				/* usb control message mode switch index */
	int um_switch_on;				/* usb control message mode switch on */
	int um_switch_off;				/* usb control message mode switch off */
	struct BCM5974_PARAM p;			/* finger pressure limits */
	struct BCM5974_PARAM w;			/* finger width limits */
	struct BCM5974_PARAM x;			/* horizontal limits */
	struct BCM5974_PARAM y;			/* vertical limits */
	struct BCM5974_PARAM o;			/* orientation limits */
};

#define DATAFORMAT(type)				\
	type,						\
	HEADER_##type,					\
	HEADER_##type + (MAX_FINGERS) * (FSIZE_##type),	\
	BUTTON_##type,					\
	FSIZE_##type,					\
	DELTA_##type,					\
	USBMSG_##type

/* logical signal quality */
#define SN_PRESSURE	45		/* pressure signal-to-noise ratio */
#define SN_WIDTH	25		/* width signal-to-noise ratio */
#define SN_COORD	250		/* coordinate signal-to-noise ratio */
#define SN_ORIENT	10		/* orientation signal-to-noise ratio */

#define PRESSURE_QUALIFICATION_THRESHOLD 2
#define SIZE_QUALIFICATION_THRESHOLD 9
#define SIZE_MU_LOWER_THRESHOLD 5

#define PRESSURE_MU_QUALIFICATION_THRESHOLD_TOTAL 15
#define SIZE_MU_QUALIFICATION_THRESHOLD_TOTAL 25

/* device constants */
static const struct BCM5974_CONFIG Bcm5974ConfigTable[] = {
	{
		USB_DEVICE_ID_APPLE_WELLSPRING_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING_JIS,
		0,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE1),
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 5342 },
		{ SN_COORD, -172, 5820 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING2_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING2_JIS,
		0,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE1),
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 4824 },
		{ SN_COORD, -172, 4290 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING3_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING3_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4460, 5166 },
		{ SN_COORD, -75, 6700 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4616, 5112 },
		{ SN_COORD, -142, 5234 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4415, 5050 },
		{ SN_COORD, -55, 6680 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x81, DATAFORMAT(TYPE2),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING8_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING8_JIS,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x83, DATAFORMAT(TYPE3),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING9_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING9_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING9_JIS,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x83, DATAFORMAT(TYPE4),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4828, 5345 },
		{ SN_COORD, -203, 6803 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_MAGICTRACKPAD2,
		USB_DEVICE_ID_APPLE_MAGICTRACKPAD2,
		USB_DEVICE_ID_APPLE_MAGICTRACKPAD2,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x83, DATAFORMAT(TYPE5),
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -3678, 3934 },
		{ SN_COORD, -2479, 2586 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_T2,
		USB_DEVICE_ID_APPLE_T2,
		USB_DEVICE_ID_APPLE_T2,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct TRACKPAD_BUTTON_DATA),
		0x83, DATAFORMAT(TYPE4),
		{ SN_PRESSURE, 0, 300 },
		/* This is incorrect; check actual data later */
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4828, 5345 },
		{ SN_COORD, -203, 6803 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
};