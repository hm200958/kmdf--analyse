/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/
#include "Define.h"
#include "device.h"
#include "queue.h"
#include "trace.h"

//
// WDFDRIVER Events
//
EXTERN_C_START
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP KMDFEvtDriverContextCleanup;
EXTERN_C_END