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
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_AmtPtpDeviceUsbKm,
    0x4aa332cc,0x5777,0x4afd,0xaa,0x4e,0x95,0x38,0x73,0x30,0x61,0x2a);
// {4aa332cc-5777-4afd-aa4e-95387330612a}
