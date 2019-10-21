#pragma once

#define SPI_TRACKPAD_MAX_FINGERS 10

typedef struct _SPI_TRACKPAD_FINGER
{
	SHORT OriginalX;
	SHORT OriginalY;
	SHORT X;
	SHORT Y;
	SHORT HorizontalAccel;
	SHORT VerticalAccel;
	SHORT ToolMajor;
	SHORT ToolMinor;
	SHORT Orientation;
	SHORT TouchMajor;
	SHORT TouchMinor;
	SHORT Rsvd1;
	SHORT Rsvd2;
	SHORT Pressure;
	SHORT Rsvd3;
} SPI_TRACKPAD_FINGER, *PSPI_TRACKPAD_FINGER;

typedef struct _SPI_TRACKPAD_PACKET
{
	UINT8 PacketType;
	UINT8 ClickOccurred;
	UINT8 Reserved0[5];
	UINT8 IsFinger;
	UINT8 Reserved1[16];
	UINT8 FingerDataLength;
	UINT8 Reserved2[5];
	UINT8 NumOfFingers;
	UINT8 ClickOccurred2;
	UINT8 State1;
	UINT8 State2;
	UINT8 State3;
	UINT8 Padding;
	UINT8 Reserved3[10];
	SPI_TRACKPAD_FINGER Fingers[SPI_TRACKPAD_MAX_FINGERS];
} SPI_TRACKPAD_PACKET, *PSPI_TRACKPAD_PACKET;

typedef struct _SPI_SET_FEATURE {
	UINT8 BusLocation;
	UINT8 Status;
} SPI_SET_FEATURE, *PSPI_SET_FEATURE;

#define HID_REPORTID_MOUSE  2
#define HID_XFER_PACKET_SIZE 255

static const SPI_TRACKPAD_INFO SpiTrackpadConfigTable[] = 
{
	/* MacBookPro11,1 / MacBookPro12,1 */
	{ 0x05ac, 0x0272, -4750, 5280, -150, 6730 },
	{ 0x05ac, 0x0273, -4750, 5280, -150, 6730 },
	/* MacBook9 */
	{ 0x05ac, 0x0275, -5087, 5579, -128, 6089 },
	/* MacBookPro14,1 / MacBookPro14,2 */
	{ 0x05ac, 0x0276, -6243, 6749, -170, 7685 },
	{ 0x05ac, 0x0277, -6243, 6749, -170, 7685 },
	/* MacBookPro14,3 */
	{ 0x05ac, 0x0278, -7456, 7976, -163, 9283 },
	/* MacBook10 */
	{ 0x05ac, 0x0279, -5087, 5579, -128, 6089 },
	/* MacBookAir7,2 fallback */
	{ 0x05ac, 0x0290, -5087, 5579, -128, 6089 },
	{ 0x05ac, 0x0291, -5087, 5579, -128, 6089 },
	/* Terminator */
	{ 0 }
};
