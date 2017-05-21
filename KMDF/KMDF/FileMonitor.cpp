#include "FileMonitor.h"
#include "MonitorCallBack.h"

CFileMonitor::CFileMonitor( CListContainer *pList ):CObjectInterface(pList)
{
	m_pFileRegFilter = nullptr;
}
CFileMonitor::~CFileMonitor( )
{

}
bool CFileMonitor::init( )
{
	FLT_OPERATION_REGISTRATION operationCall[]=
	{
		{ IRP_MJ_CREATE ,0, &FilePreCallback,&FilePostCallback,nullptr},
		{ IRP_MJ_INTERNAL_DEVICE_CONTROL ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_DEVICE_CONTROL ,0, &FilePreCallback,&FilePostCallback,nullptr },//м╛ио
		{ IRP_MJ_DIRECTORY_CONTROL ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_FILE_SYSTEM_CONTROL ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_LOCK_CONTROL ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_PNP ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_QUERY_EA ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_QUERY_INFORMATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_QUERY_QUOTA ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_QUERY_SECURITY ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_QUERY_VOLUME_INFORMATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_READ ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SET_EA ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SET_INFORMATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SET_QUOTA ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SET_SECURITY ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SET_VOLUME_INFORMATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_WRITE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_ACQUIRE_FOR_MOD_WRITE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_MDL_READ ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_MDL_READ_COMPLETE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_NETWORK_QUERY_OPEN ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_PREPARE_MDL_WRITE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_RELEASE_FOR_MOD_WRITE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_VOLUME_MOUNT ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_ACQUIRE_FOR_CC_FLUSH ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_CLEANUP ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_CLOSE ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_FLUSH_BUFFERS ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_RELEASE_FOR_CC_FLUSH ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_SHUTDOWN ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{ IRP_MJ_VOLUME_DISMOUNT ,0, &FilePreCallback,&FilePostCallback,nullptr },
		{IRP_MJ_OPERATION_END,0,nullptr,nullptr,nullptr }
	};
	FLT_REGISTRATION fltReg = { 0 };
	fltReg.Size = sizeof( FLT_REGISTRATION );
	fltReg.Version = FLT_REGISTRATION_VERSION;
	fltReg.Flags = 0;
	fltReg.ContextRegistration = nullptr;
	fltReg.OperationRegistration = operationCall;
	fltReg.InstanceSetupCallback = FileInstanceSetup;
	fltReg.InstanceQueryTeardownCallback = FileInstanceQueryTeardown;
	fltReg.InstanceTeardownStartCallback = FileInstanceTeardownStart;
	fltReg.InstanceTeardownCompleteCallback = NPInstanceTeardownComplete;
	fltReg.GenerateFileNameCallback = nullptr;
	fltReg.FilterUnloadCallback = FileUnloadCallback;
	fltReg.NormalizeContextCleanupCallback = nullptr;
	fltReg.NormalizeNameComponentCallback = nullptr;
	fltReg.TransactionNotificationCallback = nullptr;
	fltReg.NormalizeNameComponentExCallback = nullptr;

	NTSTATUS status = FltRegisterFilter( g_DvriverObject, &fltReg,&m_pFileRegFilter );
	if ( NT_SUCCESS( status ))
	{
		status = FltStartFiltering( m_pFileRegFilter );
		if ( NT_SUCCESS(status ))
		{
			return true;
		}

		FltUnregisterFilter( m_pFileRegFilter );
	}
	else
	{
		KdPrint( ( " CFileMonitor::init failed %0x\n", status ) );
		
	}
	return false;
}
void CFileMonitor::Release( )
{
	if ( m_pFileRegFilter != nullptr )
	{
		FltUnregisterFilter( m_pFileRegFilter );
	}
}
void CFileMonitor::FileMonitorData( PFLT_CALLBACK_DATA  pData,PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, bool bPre )
{
	UNREFERENCED_PARAMETER( FltObjects );
	UNREFERENCED_PARAMETER( CompletionContext );
	if ( bPre )
	{
		PreFileMonitorData( pData );
	}
	else
	{
		PostFileMonitorData( pData );
	}
	KdPrint( ( "CFileMonitor::FileMonitorData\n" ) );
}
void CFileMonitor::PreFileMonitorData( PFLT_CALLBACK_DATA  pData )
{
	if ( ( pData != nullptr ))
	{
		PFLT_IO_PARAMETER_BLOCK pIop = pData->Iopb;

		switch ( pIop->MajorFunction )
		{
			case IRP_MJ_CREATE:
				{
					AnalyseFileData( pData );
				}break;
			case IRP_MJ_INTERNAL_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DIRECTORY_CONTROL:
				{

				}break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				{

				}break;
			case IRP_MJ_LOCK_CONTROL:
				{

				}break;
			case IRP_MJ_PNP:
				{

				}break;
			case IRP_MJ_QUERY_EA:
				{

				}break;
			case IRP_MJ_QUERY_INFORMATION:
				{

				}break;
			case IRP_MJ_QUERY_QUOTA:
				{

				}break;
			case IRP_MJ_QUERY_SECURITY:
				{

				}break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_READ:
				{
					AnalyseFileData( pData );
				}break;
			case IRP_MJ_SET_EA:
				{

				}break;
			case IRP_MJ_SET_INFORMATION:
				{
					AnalyseFileData( pData );
				}break;
			case IRP_MJ_SET_QUOTA:
				{

				}break;
			case IRP_MJ_SET_SECURITY:
				{

				}break;
			case IRP_MJ_SET_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_WRITE:
				{
					AnalyseFileData( pData );
				}break;
			case IRP_MJ_ACQUIRE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:
				{

				}break;
			case IRP_MJ_MDL_READ:
				{

				}break;
			case IRP_MJ_MDL_READ_COMPLETE:
				{

				}break;
			case IRP_MJ_NETWORK_QUERY_OPEN:
				{

				}break;
			case IRP_MJ_PREPARE_MDL_WRITE:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_VOLUME_MOUNT:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_CLEANUP:
				{

				}break;
			case IRP_MJ_CLOSE:
				{
					AnalyseFileData( pData );
				}break;
			case IRP_MJ_FLUSH_BUFFERS:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_SHUTDOWN:
				{

				}break;
			case IRP_MJ_VOLUME_DISMOUNT:
				{

				}break;
			default:
				break;
		}
	}
}
void CFileMonitor::PostFileMonitorData( PFLT_CALLBACK_DATA  pData )
{
	if ( ( pData != nullptr ))
	{
		PFLT_IO_PARAMETER_BLOCK pIop = pData->Iopb;

		switch ( pIop->MajorFunction )
		{
			case IRP_MJ_CREATE:
				{
				}break;
			case IRP_MJ_INTERNAL_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DIRECTORY_CONTROL:
				{

				}break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				{

				}break;
			case IRP_MJ_LOCK_CONTROL:
				{

				}break;
			case IRP_MJ_PNP:
				{

				}break;
			case IRP_MJ_QUERY_EA:
				{

				}break;
			case IRP_MJ_QUERY_INFORMATION:
				{

				}break;
			case IRP_MJ_QUERY_QUOTA:
				{

				}break;
			case IRP_MJ_QUERY_SECURITY:
				{

				}break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_READ:
				{
				}break;
			case IRP_MJ_SET_EA:
				{

				}break;
			case IRP_MJ_SET_INFORMATION:
				{
				}break;
			case IRP_MJ_SET_QUOTA:
				{

				}break;
			case IRP_MJ_SET_SECURITY:
				{

				}break;
			case IRP_MJ_SET_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_WRITE:
				{
				}break;
			case IRP_MJ_ACQUIRE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:
				{

				}break;
			case IRP_MJ_MDL_READ:
				{

				}break;
			case IRP_MJ_MDL_READ_COMPLETE:
				{

				}break;
			case IRP_MJ_NETWORK_QUERY_OPEN:
				{

				}break;
			case IRP_MJ_PREPARE_MDL_WRITE:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_VOLUME_MOUNT:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_CLEANUP:
				{

				}break;
			case IRP_MJ_CLOSE:
				{
				}break;
			case IRP_MJ_FLUSH_BUFFERS:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_SHUTDOWN:
				{

				}break;
			case IRP_MJ_VOLUME_DISMOUNT:
				{

				}break;
			default:
				break;
		}
	}
}
bool CFileMonitor::GetDataTypeByIoParameter( const PFLT_IO_PARAMETER_BLOCK  pData, DataType &eType, FileDataType &eFileType, UINT32 &uFileAttribute, BYTE *&pRetrnData, UINT32 &uReturnLegth )
{
	bool bRet = false;

	eType = DATA_TYPE_FILE_START;
	pRetrnData = nullptr;
	uReturnLegth = 0;
	eFileType = FILE_DATATYPE_min;
	uFileAttribute = 0;

	if ( pData != nullptr )
	{
		switch ( pData->MajorFunction )
		{

			case IRP_MJ_CREATE:
				{
					if ( ( ( pData->Parameters.Create.Options >> 24 )&( FILE_SUPERSEDE ) ) || ( ( pData->Parameters.Create.Options >> 24 )&( FILE_CREATE ) ) )//create
					{
						if ( ( ( pData->Parameters.Create.Options << 8 )&( FILE_DIRECTORY_FILE << 8 ) ) )
						{
							eType = DATA_TYPE_FILE_Create_Dir;
						}
						else
						{
							eType = DATA_TYPE_FILE_Create;
						}
					}
					else //open
					{
						if ( ( ( pData->Parameters.Create.Options << 8 )&( FILE_DIRECTORY_FILE << 8 ) ) )
						{
							eType = DATA_TYPE_FILE_Open_Dir;
						}
						else
						{
							eType = DATA_TYPE_FILE_Open;
						}
					}
					
					uFileAttribute = pData->Parameters.Create.FileAttributes;

					bRet = true;

				}break;
			case IRP_MJ_INTERNAL_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DEVICE_CONTROL:
				{

				}break;
			case IRP_MJ_DIRECTORY_CONTROL:
				{

				}break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				{

				}break;
			case IRP_MJ_LOCK_CONTROL:
				{

				}break;
			case IRP_MJ_PNP:
				{

				}break;
			case IRP_MJ_QUERY_EA:
				{

				}break;
			case IRP_MJ_QUERY_INFORMATION:
				{

				}break;
			case IRP_MJ_QUERY_QUOTA:
				{

				}break;
			case IRP_MJ_QUERY_SECURITY:
				{

				}break;
			case IRP_MJ_QUERY_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_READ:
				{
					eType = DATA_TYPE_FILE_Read;
				}break;
			case IRP_MJ_SET_EA:
				{

				}break;
			case IRP_MJ_SET_INFORMATION:
				{
					eType = DATA_TYPE_FILE_Set_Information;
					pRetrnData = (BYTE *)pData->Parameters.SetFileInformation.InfoBuffer;
					uReturnLegth = pData->Parameters.SetFileInformation.Length;
					
					switch ( pData->Parameters.SetFileInformation.FileInformationClass )
					{
						case FileAllocationInformation:
							{
								eFileType = FILE_DATATYPE_FileAllocationInformation;
							}break;
						case FileBasicInformation:
							{
								eFileType = FILE_DATATYPE_FileBasicInformation;
							}break;
						case FileDispositionInformation:
							{
								pRetrnData = nullptr;
								uReturnLegth = 0;
								eType = DATA_TYPE_FILE_Delete;
								eFileType = FILE_DATATYPE_FileDispositionInformation;
							}break;
						case FileEndOfFileInformation:
							{
								eFileType = FILE_DATATYPE_FileEndOfFileInformation;
							}break;
						case FileLinkInformation:
							{
								eFileType = FILE_DATATYPE_FileLinkInformation;
							}break;
						case FilePositionInformation:
							{
								eFileType = FILE_DATATYPE_FilePositionInformation;
							}break;
						case FileRenameInformation:
							{
								eType = DATA_TYPE_FILE_Rename;
								PFILE_RENAME_INFORMATION pinfo = ( PFILE_RENAME_INFORMATION )pData->Parameters.SetFileInformation.InfoBuffer;
								if ( pinfo != nullptr )
								{
									pRetrnData = (BYTE *)pinfo->FileName;
									uReturnLegth = pinfo->FileNameLength;
								}
								eFileType = FILE_DATATYPE_FileRenameInformation;
							}break;
						case FileValidDataLengthInformation:
							{
								eFileType = FILE_DATATYPE_FileValidDataLengthInformation;
							}break;
						default:
							break;
					}
					
				}break;
			case IRP_MJ_SET_QUOTA:
				{

				}break;
			case IRP_MJ_SET_SECURITY:
				{

				}break;
			case IRP_MJ_SET_VOLUME_INFORMATION:
				{

				}break;
			case IRP_MJ_WRITE:
				{
					eType = DATA_TYPE_FILE_Write;
				}break;
			case IRP_MJ_ACQUIRE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE:
				{

				}break;
			case IRP_MJ_MDL_READ:
				{

				}break;
			case IRP_MJ_MDL_READ_COMPLETE:
				{

				}break;
			case IRP_MJ_NETWORK_QUERY_OPEN:
				{

				}break;
			case IRP_MJ_PREPARE_MDL_WRITE:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_MOD_WRITE:
				{

				}break;
			case IRP_MJ_VOLUME_MOUNT:
				{

				}break;
			case IRP_MJ_ACQUIRE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_CLEANUP:
				{

				}break;
			case IRP_MJ_CLOSE:
				{
					eType = DATA_TYPE_FILE_Close;
				}break;
			case IRP_MJ_FLUSH_BUFFERS:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_CC_FLUSH:
				{

				}break;
			case IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION:
				{

				}break;
			case IRP_MJ_SHUTDOWN:
				{

				}break;
			case IRP_MJ_VOLUME_DISMOUNT:
				{

				}break;
			default:
				break;
		}
	}
		return bRet;
}
void CFileMonitor::AnalyseFileData( PFLT_CALLBACK_DATA  pData )
{
	if ( pData != nullptr )
	{
		DataType eType = DATA_TYPE_FILE_START;
		FileDataType eFileType = FILE_DATATYPE_min;
		UINT32 uFileAttribute = 0;
		BYTE *pRetrnData = nullptr;
		UINT32 uReturnLegth = 0;

		PFLT_IO_PARAMETER_BLOCK pIop = pData->Iopb;
		PFLT_FILE_NAME_INFORMATION pFileName = nullptr;
		if ( NT_SUCCESS( FltGetFileNameInformation( pData, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &pFileName ) ) )
		{
			if ( NT_SUCCESS( FltParseFileNameInformation( pFileName ) ) )
			{
				GetDataTypeByIoParameter( pIop, eType, eFileType, uFileAttribute, pRetrnData, uReturnLegth );
				FormatFileMonitorData( pFileName->Name, eType, eFileType, uFileAttribute, pRetrnData, uReturnLegth );
				
				pRetrnData = nullptr;
			}

			FltReleaseFileNameInformation( pFileName );
		}
	}
}
void CFileMonitor::FormatFileMonitorData( UNICODE_STRING &pFileName, DataType eType, FileDataType eFileType,UINT32 uFileAttribute, BYTE *pData, UINT32 uDataLength )
{
	if ( pFileName.Length > 0 )
	{
		UINT32 uTotal = sizeof( FILEOPERATIONMONITOR_DATA );
		uTotal += pFileName.Length + 2;
		if ( uDataLength > 0 )
		{
			uTotal += uDataLength + 2;
		}

		PFILEOPERATIONMONITOR_DATA pBuffer = ( PFILEOPERATIONMONITOR_DATA )GetDataBuffer( uTotal );
		if ( pBuffer != nullptr )
		{
			pBuffer->unit.eType = eType;// GetDataTypeByIoParameter( pIop );
			pBuffer->unit.nLength = uTotal;
			pBuffer->uProceId = ( UINT32 )PsGetCurrentProcessId( );
			pBuffer->uTimes = 0;
			pBuffer->eAttType = eFileType;
			pBuffer->uFileAttribute =uFileAttribute;
			pBuffer->uFileNameOffset = sizeof( FILEOPERATIONMONITOR_DATA );
			RtlCopyMemory( ++pBuffer, pFileName.Buffer, pFileName.Length );
			--pBuffer;

			if ( pData != nullptr )
			{
				pBuffer->uDatalength = uDataLength;
				RtlCopyMemory( ( ( BYTE * )pBuffer + pBuffer->uFileNameOffset + pFileName.Length + 2 ), pData, uDataLength );
			}


			InsertDataList( ( BYTE* )pBuffer );

			KdPrint( ( "CFileMonitor::FileMonitorData -IRP_MJ_CREATE:pid =%d,FIle=%wZ", pBuffer->uProceId, pFileName ) );
		}
	}
}