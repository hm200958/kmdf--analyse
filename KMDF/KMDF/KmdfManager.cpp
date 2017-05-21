#include "KmdfManager.h"
#include "ProcessMonitor.h"
#include "RegisterMonitor.h"
#include "NetMonitor.h"
#include "FileMonitor.h"

CKmdfManager *g_CKMDFmanager = nullptr;

CKmdfManager::CKmdfManager()
{
	m_pListContainer = nullptr;
	m_pProcessMoniter = nullptr;
	m_pRegisterMoniter = nullptr;
	m_pNetmoniter = nullptr;
	m_pFileMoniter = nullptr;
}

CKmdfManager::~CKmdfManager()
{
	ReleaseObject( );
}

VOID CKmdfManager::InitManager()
{
	KdPrint(("CKmdfManager::InitManager start\n"));

	m_pListContainer = new CListContainer;
	if ( m_pListContainer != nullptr )
	{
		m_pProcessMoniter = new CProcessMonitor(m_pListContainer);
		if (m_pProcessMoniter != nullptr)
		{
			m_pProcessMoniter->init();
		}
		m_pRegisterMoniter = new CRegisterMonitor( m_pListContainer );
		if ( m_pRegisterMoniter != nullptr )
		{
			m_pRegisterMoniter->init( );
		}
		m_pNetmoniter = new CNetMonitor( m_pListContainer );
		if ( m_pNetmoniter != nullptr )
		{
			m_pNetmoniter->init( );
		}
		m_pFileMoniter = new CFileMonitor( m_pListContainer );
		if ( m_pFileMoniter != nullptr )
		{
			m_pFileMoniter->init( );
		}
	}
	
}
BYTE *CKmdfManager::GetFilterData()
{

	return NULL;
}

NTSTATUS CKmdfManager::ProcessControlIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS bRet = STATUS_SUCCESS;
	if ((DeviceObject != nullptr) && (Irp != nullptr) && ( m_pListContainer != nullptr ))
	{
		PIO_STACK_LOCATION	IrpStack = IoGetCurrentIrpStackLocation(Irp);
		if ((IrpStack != nullptr) && (IrpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL))
		{
			switch (IrpStack->Parameters.DeviceIoControl.IoControlCode)
			{
				case IOCTL_READ_DATA:
				{
					if ( ( DeviceObject->Flags & DO_DIRECT_IO ) )
					{
						bRet = ProcessReadDataBuffer( Irp, IrpStack,true );
					}
					else if ( ( DeviceObject->Flags&DO_BUFFERED_IO ))
					{
						bRet = ProcessReadDataDirect( Irp, IrpStack,true );
					}
					else
					{
						bRet = ProcessReadDataNeither( Irp, IrpStack,true );
					}

					KdPrint( ( "ProcessControlIrp read  DeviceObject->Flags=%0x\n", DeviceObject->Flags ) );
					
				}break;
			case IOCTL_WRITE_DATA:
			{
				if ( ( DeviceObject->Flags & DO_DIRECT_IO ) )
				{
					bRet = ProcessWriteDataBuffer( Irp, IrpStack,true );
				}
				else if ( ( DeviceObject->Flags&DO_BUFFERED_IO ) )
				{
					bRet = ProcessWriteDataDirect( Irp, IrpStack,true );
				}
				else
				{
					bRet = ProcessWriteDataNeither( Irp, IrpStack );
				}

				KdPrint( ( "ProcessControlIrp write DeviceObject->Flags=%0x\n", DeviceObject->Flags ) );
				//ULONG uBufferLength = IrpStack->Parameters.Write.Length;
				}break;
				default:break;
			}
		}
	}

	return bRet;
}

NTSTATUS CKmdfManager::ProcessReadDataDirect( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if ( ( Irp != nullptr ) && ( IrpStack != nullptr ) && ( m_pListContainer != nullptr ) )
	{
		PUNIT_FLAG pData = PUNIT_FLAG( m_pListContainer->GetDataFromList( false ) );
		if ( pData != nullptr )
		{
			ULONG uBufferLength = 0;
			if ( bContrl )
			{
				uBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			}
			else
			{
				uBufferLength = IrpStack->Parameters.Read.Length;
			}
			
			
			ULONG uDataLength = pData->nLength;
			if ( Irp->MdlAddress == nullptr )
			{
				Irp->IoStatus.Information = pData->nLength;
				status = STATUS_SUCCESS;
			}
			else
			{
				if ( uBufferLength < pData->nLength )
				{
					Irp->IoStatus.Information = pData->nLength;
					status = STATUS_BUFFER_OVERFLOW;
				}
				else
				{
					pData = PUNIT_FLAG( m_pListContainer->GetDataFromList( true ) );
					PVOID pBuffer = MmGetSystemAddressForMdlSafe( Irp->MdlAddress, NormalPagePriority );
					if ( pBuffer != nullptr )
					{
						CopyDataToUser( pBuffer, PUNIT_FLAG( pData ) );
						Irp->IoStatus.Information = pData->nLength;
					}
					
					m_pListContainer->FreeDataBuffer( ( BYTE* )pData );
					status = STATUS_SUCCESS;
				}
			}

			KdPrint( ( "ProcessReadData:direct=%p;bufferLength=%d,dataLength=%d\n", Irp->MdlAddress, uBufferLength, uDataLength ) );
		}
		else
		{
			Irp->IoStatus.Information = 0;
		}
	}
	return status;
}
NTSTATUS CKmdfManager::ProcessReadDataNeither( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if ( ( Irp != nullptr ) && ( IrpStack != nullptr ) && ( m_pListContainer != nullptr ) )
	{
		PUNIT_FLAG pData = PUNIT_FLAG( m_pListContainer->GetDataFromList( false ) );
		if ( pData != nullptr )
		{
			ULONG uBufferLength = 0;
			if ( bContrl )
			{
				uBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			}
			else
			{
				uBufferLength = IrpStack->Parameters.Read.Length;
			}
			ULONG uDataLength = pData->nLength;
			if ( Irp->UserBuffer == nullptr )
			{
				Irp->IoStatus.Information = pData->nLength;
				status = STATUS_SUCCESS;
			}
			else
			{
				if ( uBufferLength < pData->nLength )
				{
					Irp->IoStatus.Information = pData->nLength;
					status = STATUS_BUFFER_OVERFLOW;
				}
				else
				{
					PVOID pBuffer = Irp->UserBuffer;
					__try
					{
						ProbeForWrite( pBuffer, uBufferLength, 1 );
						pData = PUNIT_FLAG( m_pListContainer->GetDataFromList( true ) );

						CopyDataToUser( pBuffer, PUNIT_FLAG( pData ) );
						Irp->IoStatus.Information = pData->nLength;

						m_pListContainer->FreeDataBuffer( ( BYTE* )pData );
						status = STATUS_SUCCESS;

						KdPrint( ( "ProcessReadData:userbuffer=%p;bufferLength=%d,dataLength=%d\n", pBuffer, uBufferLength, uDataLength ) );
					}
					__except ( EXCEPTION_EXECUTE_HANDLER)
					{
						KdPrint( ( "ProcessReadDataNeither falsed\n" ) );
						status = STATUS_UNSUCCESSFUL;
					}	
				}
			}
		}
		else
		{
			Irp->IoStatus.Information = 0;
		}
	}
	return status;
}
NTSTATUS CKmdfManager::ProcessReadDataBuffer(IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if (( Irp != nullptr )&&( IrpStack != nullptr )&&( m_pListContainer != nullptr ))
	{
		PUNIT_FLAG pData = PUNIT_FLAG(m_pListContainer->GetDataFromList(false));
		if (pData != nullptr)
		{
			ULONG uBufferLength = 0;
			if ( bContrl )
			{
				uBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			}
			else
			{
				uBufferLength = IrpStack->Parameters.Read.Length;
			}
			ULONG uDataLength = pData->nLength;
			if ( Irp->AssociatedIrp.SystemBuffer == nullptr )
			{
				Irp->IoStatus.Information = pData->nLength;
				status = STATUS_SUCCESS;
			}
			else
			{
				if (uBufferLength < pData->nLength)
				{
					Irp->IoStatus.Information = pData->nLength;
					status = STATUS_BUFFER_OVERFLOW;
				}
				else
				{
					pData = PUNIT_FLAG(m_pListContainer->GetDataFromList(true));
					CopyDataToUser(Irp->AssociatedIrp.SystemBuffer, PUNIT_FLAG(pData));
					Irp->IoStatus.Information = pData->nLength;

					m_pListContainer->FreeDataBuffer((BYTE*)pData);
					status = STATUS_SUCCESS;
				}
			}

			KdPrint( ( "ProcessReadData:systembuff=%p;bufferLength=%d,dataLength=%d\n", Irp->AssociatedIrp.SystemBuffer, uBufferLength, uDataLength ) );
		}
		else
		{
			Irp->IoStatus.Information = 0;
		}
	}
	return status;
}
NTSTATUS CKmdfManager::ProcessWriteDataBuffer(IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if ((Irp != nullptr) && (IrpStack != nullptr))
	{
		ULONG uBufferLength = 0;
		if ( bContrl )
		{
			uBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		}
		else
		{
			uBufferLength = IrpStack->Parameters.Write.Length;
		}
	}
	return status;
}
NTSTATUS CKmdfManager::ProcessWriteDataDirect( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if ( ( Irp != nullptr ) && ( IrpStack != nullptr ) )
	{
		ULONG uBufferLength = 0;
		if ( bContrl )
		{
			uBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		}
		else
		{
			uBufferLength = IrpStack->Parameters.Write.Length;
		}
	}
	return status;
}
NTSTATUS CKmdfManager::ProcessWriteDataNeither( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl )
{
	NTSTATUS status = STATUS_SUCCESS;
	if ( ( Irp != nullptr ) && ( IrpStack != nullptr ) )
	{
		ULONG uBufferLength = 0;
		if ( bContrl )
		{
			uBufferLength = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		}
		else
		{
			uBufferLength = IrpStack->Parameters.Write.Length;
		}
	}
	return status;
}
void CKmdfManager::ProcessMonitor(__in HANDLE  ProcessId,const PPS_CREATE_NOTIFY_INFO pCreateInfo)
{
	if ( m_pProcessMoniter != nullptr )
	{
		m_pProcessMoniter->ProcessMonitorData(ProcessId, pCreateInfo);
	}
}
void CKmdfManager::RegisterMonitor( REG_NOTIFY_CLASS regType, const PVOID pData )
{
	if ( m_pRegisterMoniter != nullptr )
	{
		m_pRegisterMoniter->RegisterMonitorData( regType, pData );
	}

}
 void CKmdfManager::NetMoniter( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData, OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType )
 {
 	if ( m_pNetmoniter != nullptr )
 	{
 		m_pNetmoniter->NetMonitorData( inFixedValues, inMetaValues, pNetrData, classifyOut,eType );
 	}
 
 }
 void CKmdfManager::FileMonitor( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext,bool bPre )
 {
	 if ( m_pFileMoniter != nullptr )
	 {
		 m_pFileMoniter->FileMonitorData( pData, FltObjects, CompletionContext, bPre );
	 }
 }
void CKmdfManager::ReleaseObject( )
{
	if ( m_pProcessMoniter != nullptr )
	{
		delete m_pProcessMoniter;
		m_pProcessMoniter = nullptr;
	}
	if ( m_pRegisterMoniter !=  nullptr )
	{
		delete m_pRegisterMoniter;
		m_pRegisterMoniter = nullptr;
	}
	if ( m_pNetmoniter != nullptr )
	{
		delete m_pNetmoniter;
		m_pNetmoniter = nullptr;
	}
	if ( m_pFileMoniter != nullptr )
	{
		delete m_pFileMoniter;
		m_pFileMoniter = nullptr;
	}
	if ( m_pListContainer != nullptr )
	{
		delete m_pListContainer;
		m_pListContainer = nullptr;
	}
	
}
void CKmdfManager::CopyDataToUser(PVOID const Buffer, PUNIT_FLAG const pData)
{
	if (( Buffer != nullptr ) && ( pData != nullptr ))
	{
		switch (pData->eType)
		{
		case DataType::DATA_TYPE_PROCESS_Create:
		{
			pData->nLength -= sizeof(UNIT_Flag::ListEntry);

			if ( PPROCESS_DATA(pData)->uCMDLineOffset > 0 )
			{
				PPROCESS_DATA(pData)->uCMDLineOffset -= sizeof(UNIT_Flag::ListEntry);
			}
			if ( PPROCESS_DATA(pData)->uParentFileOffset > 0 )
			{
				PPROCESS_DATA(pData)->uParentFileOffset -= sizeof(UNIT_Flag::ListEntry);
			}
			if ( PPROCESS_DATA(pData)->uProcessFileOffset > 0)
			{
				PPROCESS_DATA(pData)->uProcessFileOffset -= sizeof(UNIT_Flag::ListEntry);
			}

			RtlCopyMemory(Buffer, ((BYTE*)pData) + sizeof(UNIT_Flag::ListEntry), pData->nLength );

		}break;
		case DataType::DATA_TYPE_PROCESS_Exit:
		{
			pData->nLength -= sizeof( UNIT_Flag::ListEntry );
			RtlCopyMemory( Buffer, ( ( BYTE* )pData ) + sizeof( UNIT_Flag::ListEntry ), pData->nLength );
		}break;
		case DataType::DATA_TYPE_REGISTER_CreateKey:
		case DataType::DATA_TYPE_REGISTER_CreateKeyEX:
		case DataType::DATA_TYPE_REGISTER_DeleteKey:
		case DataType::DATA_TYPE_REGISTER_SetValueKey:
		case DataType::DATA_TYPE_REGISTER_DeleteValueKey:
		case DataType::DATA_TYPE_REGISTER_QueryKey:
		case DataType::DATA_TYPE_REGISTER_QueryValueKey:
		case DataType::DATA_TYPE_REGISTER_OpenKey:
		case DataType::DATA_TYPE_REGISTER_OpenKeyEx:
		{
				pData->nLength -= sizeof( UNIT_Flag::ListEntry );

				if ( PREGISTER_DATA( pData )->uRegPathOffset > 0 )
				{
					PREGISTER_DATA( pData )->uRegPathOffset -= sizeof( UNIT_Flag::ListEntry );
				}
				if ( PREGISTER_DATA( pData )->uRegNameOffset > 0 )
				{
					PREGISTER_DATA( pData )->uRegNameOffset -= sizeof( UNIT_Flag::ListEntry );
				}
				if ( PREGISTER_DATA( pData )->uRegDataOffset > 0 )
				{
					PREGISTER_DATA( pData )->uRegDataOffset -= sizeof( UNIT_Flag::ListEntry );
				}

				RtlCopyMemory( Buffer, ( ( BYTE* )pData ) + sizeof( UNIT_Flag::ListEntry ), pData->nLength );
		}break;
		case DATA_TYPE_FILE_Create://文件创建
		case DATA_TYPE_FILE_Create_Dir:
		case DATA_TYPE_FILE_Write:
		case DATA_TYPE_FILE_Read:
		case DATA_TYPE_FILE_Set_Information:
		case DATA_TYPE_FILE_Delete:
		case DATA_TYPE_FILE_Rename:
		case DATA_TYPE_FILE_Open:
		case DATA_TYPE_FILE_Open_Dir:
		case DATA_TYPE_FILE_Close:
		{
				pData->nLength -= sizeof( UNIT_Flag::ListEntry );
				if ( PFILEOPERATIONMONITOR_DATA( pData )->uFileNameOffset > 0 )
				{
					PFILEOPERATIONMONITOR_DATA( pData )->uFileNameOffset -= sizeof( UNIT_Flag::ListEntry );
				}
				if ( PFILEOPERATIONMONITOR_DATA( pData )->uDataOffset > 0 )
				{
					PFILEOPERATIONMONITOR_DATA( pData )->uDataOffset -= sizeof( UNIT_Flag::ListEntry );
				}
				RtlCopyMemory( Buffer, ( ( BYTE* )pData ) + sizeof( UNIT_Flag::ListEntry ), pData->nLength );
		}break;
		case DATA_TYPE_NET_Connect:
		case DATA_TYPE_NET_Bind:
		case DATA_TYPE_NET_DNS:
			{
				pData->nLength -= sizeof( UNIT_Flag::ListEntry );
				if ( PNETWORKMONITOR_DATA(pData)->uNetDataOffset > 0 )
				{
					PNETWORKMONITOR_DATA( pData )->uNetDataOffset -= sizeof( UNIT_Flag::ListEntry );
				}
				RtlCopyMemory( Buffer, ( ( BYTE* )pData ) + sizeof( UNIT_Flag::ListEntry ), pData->nLength );
			}break;
		default:
			break;
		}
	}
}