/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_AmtPtpDeviceSpiKm,
    0xf271e39d,0xd211,0x4a6e,0xa4,0x39,0x63,0xe5,0x19,0x40,0xd2,0x10);
// {f271e39d-d211-4a6e-a439-63e51940d210}
