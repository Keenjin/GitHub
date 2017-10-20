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

DEFINE_GUID (GUID_DEVINTERFACE_ramdisk,
    0xdaacedfd,0x04e6,0x4150,0x88,0xfa,0x29,0x01,0x0e,0xca,0xd2,0x5d);
// {daacedfd-04e6-4150-88fa-29010ecad25d}
