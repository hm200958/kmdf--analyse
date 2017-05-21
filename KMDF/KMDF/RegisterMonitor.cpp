#include <ntifs.h>
#include "RegisterMonitor.h"
#include "MonitorCallBack.h"

CRegisterMonitor::CRegisterMonitor( CListContainer *pList ):CObjectInterface( pList )
{
	m_CallCooke = LARGE_INTEGER{ 0};
}
CRegisterMonitor::~CRegisterMonitor( )
{
	//Release( );
}
bool CRegisterMonitor::init( )
{
	NTSTATUS status = CmRegisterCallback( RegistryCallback, nullptr, &m_CallCooke );
	if ( !NT_SUCCESS(status))
	{
		KdPrint( ( "CRegisterMonitor RegistryCallback falied! status=%d\n",status));
		return false;
	}
	KdPrint( ( "CRegisterMonitor RegistryCallback success\n") );
	return true;
}
void CRegisterMonitor::Release( )
{
	CmUnRegisterCallback( m_CallCooke );
	__super::Release( );
}
void CRegisterMonitor::RegisterMonitorData( REG_NOTIFY_CLASS regType, const PVOID pData  )
{
	if (( m_pListContainer != nullptr ) && ( pData != nullptr ))
	{
		switch ( regType )
		{
		case RegNtPreDeleteKey:
		{
			FilterDeleteKey( pData );
		}break;
		case RegNtPreSetValueKey:
		{
			FilterSetValueKey( pData );
		}break;
		case RegNtPreDeleteValueKey:
		{
			FilterDeleteValueKey( pData );
		}break;
		case RegNtPreSetInformationKey:
		{

		}break;
		case RegNtPreRenameKey:
		{

		}break;
		case RegNtPreEnumerateKey:
		{

		}break;
		case RegNtPreEnumerateValueKey:
		{

		}break;
		case RegNtPreQueryKey:
		{
			FilterQueryKey( pData );
		}break;
		case RegNtPreQueryValueKey:
		{
			FilterQueryValueKey( pData );
		}break;
		case RegNtPreQueryMultipleValueKey:
		{

		}break;
		case RegNtPreCreateKey:
		{
			FilterCreateKey( pData );
		}break;
		case RegNtPreOpenKey:
		{
			FilterOpenKey( pData );
		}break;
		case RegNtPreKeyHandleClose:
		{

		}break;
		case RegNtPreCreateKeyEx:
		{
			FilterCreateKeyEx( pData );
		}break;
		case RegNtPreOpenKeyEx:
		{
			FilterOpenKeyEx( pData );
		}break;
		case RegNtPreFlushKey:
		{

		}break;
		case RegNtPreLoadKey:
		{

		}break;
		case RegNtPreUnLoadKey:
		{

		}break;
		case RegNtPreQueryKeySecurity:
		{

		}break;
		case RegNtPreSetKeySecurity:
		{

		}break;
		case RegNtCallbackObjectContextCleanup:
		{

		}break;
		case RegNtPreRestoreKey:
		{

		}break;
		case RegNtPreSaveKey:
		{

		}break;
		case RegNtPreReplaceKey:
		{

		}break;
		case RegNtPreQueryKeyName:
		{

		}break;
		default:
			break;
		}
	}
}
bool CRegisterMonitor::GetObjectPathName( PVOID pObject, PUNICODE_STRING &pReturn )
{
	if (( pObject != nullptr ) && ( MmIsAddressValid( pObject )))
	{
		ULONG uReturnLength = 0;
		PUNICODE_STRING pName = nullptr;
		NTSTATUS status = ObQueryNameString( pObject, (POBJECT_NAME_INFORMATION)pName, 0, &uReturnLength );
		if (( status == STATUS_INFO_LENGTH_MISMATCH ))
		{
			pName = (PUNICODE_STRING)ExAllocatePoolWithTag( NonPagedPool, uReturnLength, ALLOCTAG );
			if ( pName != nullptr )
			{
				
				status = ObQueryNameString( pObject, ( POBJECT_NAME_INFORMATION )pName, uReturnLength, &uReturnLength );
				if ( NT_SUCCESS(status))
				{
					pReturn = pName;
					return true;
				}
				else
				{
					ExFreePoolWithTag( pName, ALLOCTAG );
				}
			}

		}
	}

	return false;
}
bool CRegisterMonitor::FormatDataToRegisterData( DataType eType, PUNICODE_STRING pPath, PUNICODE_STRING pValueName, PVOID pData, ULONG uDataLength, ULONG eDataType)
{
	ULONG uPath = 0;
	ULONG uName = 0;
	ULONG uTotal = sizeof( REGISTER_DATA );
	if (( pPath != nullptr ))
	{
		uPath = pPath->Length + 2;
		uTotal += uPath;
	}
	
	if (( pValueName != nullptr ))
	{
		uName = pValueName->Length +2;
		uTotal += uName;
	}
	if (( pData != nullptr ) && ( uDataLength > 0 ))
	{
		uTotal += uDataLength;
		uTotal += 2;
	}
	
	PREGISTER_DATA pBuffer = ( PREGISTER_DATA )GetDataBuffer( uTotal );
	if ( pBuffer != nullptr )
	{
		__try {
			pBuffer->unit.eType = eType;
			pBuffer->unit.nLength = uTotal;
			pBuffer->uProceId = ( UINT32 )PsGetCurrentProcessId( );
			pBuffer->eRegType = ( RegDataType )eDataType;
			pBuffer->uDataLength = uDataLength;

			if ( ( pPath != nullptr ) )
			{
				pBuffer->uRegPathOffset = sizeof( REGISTER_DATA );

				++pBuffer;
				RtlCopyMemory( pBuffer, pPath->Buffer, pPath->Length );
				--pBuffer;
			}
			if ( ( pValueName != nullptr ) )
			{
				pBuffer->uRegNameOffset = sizeof( REGISTER_DATA ) + uPath;
				RtlCopyMemory( ( ( BYTE * )pBuffer ) + pBuffer->uRegNameOffset, pValueName->Buffer, pValueName->Length );
			}
			if ( ( pData != nullptr ) && ( uDataLength > 0 ) )
			{
				pBuffer->uRegDataOffset = sizeof( REGISTER_DATA ) + uPath + uName;
				RtlCopyMemory( ( ( BYTE * )pBuffer ) + pBuffer->uRegDataOffset, pData, uDataLength );
			}

			InsertDataList( ( BYTE* )pBuffer );
			KdPrint( ( "FormatDataToRegisterData:DataType=%d,path=%wZ,PID = %0x,valueName=\n", eType, pPath, pBuffer->uProceId, pValueName ) );
			return true;
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			KdPrint( ( "FormatDataToRegisterData exception:DataType=%d,path=%wZ,PID = %0x,valueName=%wZ,TOTAL=%d,Name=%d\n", eType, pPath, pBuffer->uProceId, pValueName,uTotal,uName ) );
			m_pListContainer->FreeDataBuffer( (BYTE*)pBuffer );
		}
		
	}
		
	return false;
}
void CRegisterMonitor::FilterDeleteKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;

		PREG_DELETE_KEY_INFORMATION pDelete = PREG_DELETE_KEY_INFORMATION( pData );
		if ( MmIsAddressValid( pDelete ) )
		{
			if ( GetObjectPathName( pDelete->Object, pPath ) && ( pPath != nullptr ) )
			{
				FormatDataToRegisterData( DATA_TYPE_REGISTER_DeleteKey, pPath );
				
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}
		}
	}
}
void CRegisterMonitor::FilterSetValueKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_SET_VALUE_KEY_INFORMATION pValue = ( PREG_SET_VALUE_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ))
		{
			if ( GetObjectPathName( pValue->Object,pPath ) && ( pPath != nullptr ))
			{
				FormatDataToRegisterData( DATA_TYPE_REGISTER_SetValueKey, pPath, pValue->ValueName, pValue->Data, pValue->DataSize, pValue->Type );
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}

		}
	}
}
void CRegisterMonitor::FilterDeleteValueKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_DELETE_VALUE_KEY_INFORMATION pValue = ( PREG_DELETE_VALUE_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			if ( GetObjectPathName( pValue->Object, pPath ) && ( pPath != nullptr ) )
			{
				FormatDataToRegisterData( DATA_TYPE_REGISTER_DeleteValueKey, pPath, pValue->ValueName );
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}

		}
	}
}
void CRegisterMonitor::FilterQueryKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_QUERY_KEY_INFORMATION pValue = ( PREG_QUERY_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			if ( GetObjectPathName( pValue->Object, pPath ) && ( pPath != nullptr ) )
			{
				UNICODE_STRING KeyName = { 0 };

				switch ( pValue->KeyInformationClass )
				{
				case KeyBasicInformation:
				{
					PKEY_BASIC_INFORMATION pKey = ( PKEY_BASIC_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath, nullptr, pKey, pValue->Length, REGDATATYPE_BasicClassInfo );
					}
				}break;
				case KeyNodeInformation:
				{
					PKEY_NODE_INFORMATION pKey = ( PKEY_NODE_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath, nullptr, pKey, pValue->Length, REGDATATYPE_NodeClassInfo );
					}
				}break;
				case KeyFullInformation:
				{
					PKEY_FULL_INFORMATION pKey = ( PKEY_FULL_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath, nullptr, pKey, pValue->Length, REGDATATYPE_FullClassInfo );
					}
				}break;
				case KeyNameInformation:
				{
					PKEY_NAME_INFORMATION pKey = ( PKEY_NAME_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath, nullptr, pKey,pValue->Length, REGDATATYPE_NameClassInfo );

					}
				}break;
				case KeyCachedInformation:
				{
					PKEY_CACHED_INFORMATION pKey = ( PKEY_CACHED_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath,nullptr, pKey, pValue->Length, REGDATATYPE_CachedClassInfo );

					}
				}break;
				case KeyFlagsInformation:
				{

				}break;
				case KeyVirtualizationInformation:
				{
					PKEY_VIRTUALIZATION_INFORMATION pKey = ( PKEY_VIRTUALIZATION_INFORMATION )pValue->KeyInformation;
					if ( pKey != nullptr )
					{
						FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryKey, pPath, nullptr, pKey, pValue->Length, REGDATATYPE_VirtualizationClassInfo );
					}
				}break;
				case KeyHandleTagsInformation:
				{

				}break;
				default:
					break;
				}
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}
		}
	}
}
void CRegisterMonitor::FilterQueryValueKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_QUERY_VALUE_KEY_INFORMATION pValue = ( PREG_QUERY_VALUE_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			if ( GetObjectPathName( pValue->Object, pPath ) && ( pPath != nullptr ) )
			{
				switch ( pValue->KeyValueInformationClass )
				{
					case KeyValueBasicInformation:
						{
							FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryValueKey, pPath, pValue->ValueName, pValue->KeyValueInformation, pValue->Length, REGDATATYPE_KeyValueBasic );
						}break;
					case KeyValueFullInformation:
						{
							FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryValueKey, pPath, pValue->ValueName, pValue->KeyValueInformation, pValue->Length, REGDATATYPE_KeyValueFull );
						}break;
					case KeyValuePartialInformation:
						{
							FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryValueKey, pPath, pValue->ValueName, pValue->KeyValueInformation, pValue->Length, REGDATATYPE_KeyValuePartial );
						}break;
					case KeyValueFullInformationAlign64:
						{
							FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryValueKey, pPath, pValue->ValueName, pValue->KeyValueInformation, pValue->Length, REGDATATYPE_KeyValueFullAlign64 );
						}break;
					case KeyValuePartialInformationAlign64:
						{
							FormatDataToRegisterData( DATA_TYPE_REGISTER_QueryValueKey, pPath, pValue->ValueName, pValue->KeyValueInformation, pValue->Length, REGDATATYPE_KeyValuePartialAlign64 );
						}break;
					default:
						break;
				}

				ExFreePoolWithTag( pPath, ALLOCTAG );
			}

		}
	}
}
void CRegisterMonitor::FilterCreateKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PREG_PRE_CREATE_KEY_INFORMATION pValue = ( PREG_PRE_CREATE_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			FormatDataToRegisterData( DATA_TYPE_REGISTER_CreateKey, pValue->CompleteName );
		}
	}
}
void CRegisterMonitor::FilterOpenKey( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PREG_PRE_OPEN_KEY_INFORMATION pValue = ( PREG_PRE_OPEN_KEY_INFORMATION )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			FormatDataToRegisterData( DATA_TYPE_REGISTER_OpenKey, pValue->CompleteName );
		}
	}
}
void CRegisterMonitor::FilterCreateKeyEx( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_CREATE_KEY_INFORMATION_V1 pValue = ( PREG_CREATE_KEY_INFORMATION_V1 )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			if ( GetObjectPathName( pValue->RootObject, pPath ) && ( pPath != nullptr ) )
			{
				FormatDataToRegisterData( DATA_TYPE_REGISTER_CreateKeyEX, pPath, pValue->CompleteName );
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}

		}
	}
}
void CRegisterMonitor::FilterOpenKeyEx( const PVOID pData )
{
	if ( pData != nullptr )
	{
		PUNICODE_STRING pPath = nullptr;
		PREG_OPEN_KEY_INFORMATION_V1 pValue = ( PREG_OPEN_KEY_INFORMATION_V1 )pData;
		if ( MmIsAddressValid( pValue ) )
		{
			if ( GetObjectPathName( pValue->RootObject, pPath ) && ( pPath != nullptr ) )
			{
				FormatDataToRegisterData( DATA_TYPE_REGISTER_OpenKeyEx, pPath, pValue->CompleteName );
				ExFreePoolWithTag( pPath, ALLOCTAG );
			}

		}
	}
}