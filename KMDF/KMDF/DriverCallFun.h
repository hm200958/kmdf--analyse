#pragma once
#include "Define.h"

NTSTATUS DriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DriverRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DriverWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DriverContrlProcess(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DriverIrpThrough(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID CompliteIrpProcess(IN PIRP Irp, NTSTATUS status);

VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject);

