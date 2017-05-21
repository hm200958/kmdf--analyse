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

DEFINE_GUID (GUID_DEVINTERFACE_KMDF,
    0x880ba520,0xb785,0x4e96,0x83,0x91,0xc7,0xd6,0x88,0xe3,0xbd,0x4c);
// {880ba520-b785-4e96-8391-c7d688e3bd4c}
