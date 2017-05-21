#include "DriverCallFun.h"
#include "KmdfManager.h"

NTSTATUS DriverCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint( ( "DriverCreate start\n" ) );
	NTSTATUS status = STATUS_SUCCESS;
	if ( IS_MY_DEVICE_OBJECT( DeviceObject ) || IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject))
	{
		KdPrint(("DriverCreate is myself\n"));
		//g_CKMDFmanager = new CKmdfManager;
		//if (g_CKMDFmanager != nullptr)
		//{
		//	g_CKMDFmanager->InitManager();
		//}
		CompliteIrpProcess(Irp, status);
	}
	else
	{
		KdPrint(("DriverCreate is not myself\n"));
		status = DriverIrpThrough(DeviceObject, Irp);
	}

	return status;
}
NTSTATUS DriverRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint( ( "DriverRead start\n" ) );
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS status = STATUS_SUCCESS;
	if (IS_MY_DEVICE_OBJECT(DeviceObject) || IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject))
	{
		KdPrint( ( "DriverRead ok\n" ) );
		if ( g_CKMDFmanager != nullptr )
		{
			PIO_STACK_LOCATION	IrpStack = IoGetCurrentIrpStackLocation(Irp);
			if (( DeviceObject->Flags & DO_BUFFERED_IO ))
			{
				status = g_CKMDFmanager->ProcessReadDataBuffer( Irp, IrpStack );
			}
			else if (( DeviceObject->Flags & DO_DIRECT_IO))
			{
				status = g_CKMDFmanager->ProcessReadDataDirect( Irp, IrpStack );
			}
			else
			{
				status = g_CKMDFmanager->ProcessReadDataNeither( Irp, IrpStack );
			}

			KdPrint( ( "DriverRead DeviceObject->Flags=%0x\n", DeviceObject->Flags ) );
		}
		CompliteIrpProcess(Irp, status);
	}
	else
	{
		KdPrint( ( "DriverRead false\n" ) );
		status = DriverIrpThrough(DeviceObject, Irp);
	}

	return status;
}
NTSTATUS DriverWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint( ( "DriverWrite start\n" ) );
	NTSTATUS status = STATUS_SUCCESS;
	if (IS_MY_DEVICE_OBJECT(DeviceObject) || IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject))
	{
		KdPrint( ( "DriverWrite ok\n" ) );
		if (g_CKMDFmanager != nullptr)
		{
			PIO_STACK_LOCATION	IrpStack = IoGetCurrentIrpStackLocation(Irp);
			if ( ( DeviceObject->Flags & DO_DIRECT_IO ) )
			{
				status = g_CKMDFmanager->ProcessWriteDataBuffer( Irp, IrpStack );
			}
			else if ( ( DeviceObject->Flags&DO_BUFFERED_IO ) )
			{
				status = g_CKMDFmanager->ProcessWriteDataDirect( Irp, IrpStack );
			}
			else
			{
				status = g_CKMDFmanager->ProcessWriteDataNeither( Irp, IrpStack );
			}

			KdPrint( ( "DriverWrite DeviceObject->Flags=%0x\n", DeviceObject->Flags ) );
		}
		CompliteIrpProcess(Irp, status);
	}
	else
	{
		KdPrint( ( "DriverWrite false\n" ) );
		status = DriverIrpThrough(DeviceObject, Irp);
	}

	return status;
}
VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject)
{
	ASSERT(DriverObject == g_DvriverObject);
	IoDeleteDevice(g_DvriverControlObject);
	KdPrint(("DriverUnload\n"));

	//ObDereferenceObject(g_DvriverControlObject);
	ObDereferenceObject(g_DvriverObject);
	
	g_DvriverControlObject = nullptr;
	g_DvriverObject = nullptr;
	if ( g_CKMDFmanager != nullptr)
	{
		delete g_CKMDFmanager;
	}
}
NTSTATUS DriverClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	KdPrint( ( "Driverclsoe start\n" ) );
	NTSTATUS status = STATUS_SUCCESS;
	if (IS_MY_DEVICE_OBJECT(DeviceObject) || IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject))
	{
		KdPrint( ( "Driverclsoe ok\n" ) );
		/*if (g_CKMDFmanager != nullptr)
		{
			delete g_CKMDFmanager;
		}*/
		CompliteIrpProcess(Irp, status);
	}
	else
	{
		KdPrint( ( "Driverclsoe False\n" ) );
		status = DriverIrpThrough(DeviceObject, Irp);
	}

	return status;
}
NTSTATUS DriverContrlProcess(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	if (IS_MY_DEVICE_OBJECT(DeviceObject) || IS_MY_CONTROL_DEVICE_OBJECT(DeviceObject))
	{
		KdPrint(("DriverContrlProcess:g_CKMDFmanager = %p\n",g_CKMDFmanager));
		if ( g_CKMDFmanager != nullptr)
		{
			status = g_CKMDFmanager->ProcessControlIrp( DeviceObject,Irp );

			
		}
		CompliteIrpProcess(Irp,status);
	}
	else
	{
		status = DriverIrpThrough(DeviceObject, Irp);
	}

	return status;
}
NTSTATUS DriverIrpThrough(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(DeviceObject, Irp);
}
VOID CompliteIrpProcess(IN PIRP Irp,NTSTATUS status )
{
	if ( Irp != nullptr )
	{
		Irp->IoStatus.Status = status;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}
	
}