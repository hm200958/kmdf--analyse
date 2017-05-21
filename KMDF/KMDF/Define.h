#pragma once
#include <Fltkernel.h>
#include <ntddk.h>
#include <ndis.h>
#include <fwpsk.h>
#include <wdf.h>
#include <wdm.h>
#include <initguid.h>


extern PDRIVER_OBJECT g_DvriverObject;
extern PDEVICE_OBJECT g_DvriverControlObject;

#define ALLOCTAG 'HMHM'
#define DNSPORT 53

//#define EXTERN_C_START extern"C"{
//#define EXTERN_C_END };

#define  DRIVER_NAME L"\\Device\\KMDFService"
#define  DRIVER_SYMBOLICLINK L"\\??\\KMDFDOSService"

typedef NTSTATUS (*ZwQueryInformationProcess)(_In_ HANDLE,_In_ PROCESSINFOCLASS ,_Out_ PVOID,_In_ ULONG,_Out_opt_ PULONG);

#define IS_MY_DEVICE_OBJECT(_devObj) \
    (((_devObj) != nullptr) && \
     ((_devObj)->DriverObject == g_DvriverObject))


#define IS_MY_CONTROL_DEVICE_OBJECT(_devObj) \
    (((_devObj) != nullptr)&&((_devObj) == g_DvriverControlObject) ? \
            (ASSERT(((_devObj)->DriverObject == g_DvriverObject) && \
                    ((_devObj)->DeviceExtension == NULL)), TRUE) : \
            FALSE)
