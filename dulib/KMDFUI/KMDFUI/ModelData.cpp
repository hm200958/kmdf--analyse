#include "StdAfx.h"
#include "ModelData.h"
#include "GlobalMap.h"
#include "GlobalFun.h"
CModelData::CModelData()
{
	m_hDevice = nullptr;
}
CModelData::~CModelData()
{

}
void CModelData::CloseDevice( )
{
	if ( HANDLE_SUCCESS( m_hDevice ))
	{
		m_objNetFilter.Release( );
		CloseHandle( m_hDevice );
		m_hDevice = nullptr;
	}
}
BOOL CModelData::CreateDevice( )
{
	m_hDevice = CreateFile(_T("//./KMDFDOSService"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL );
	if ( HANDLE_SUCCESS( m_hDevice ))
	{
		m_objNetFilter.init( );
		return TRUE;
	}
	return FALSE;
}
BOOL CModelData::ReadData( )
{
	BOOL bRet = FALSE;
	DWORD NeedLength = 0;
	if ( ReadData( nullptr, 0, &NeedLength ) &&( NeedLength > 0 ))
	{
		CString str;
		str.Format( _T( "ReadData lentgh %d" ), NeedLength );
		OutputDebugString( str );
		BYTE *pData = new BYTE[ NeedLength ];
		if ( ( pData != nullptr ) )
		{
			if ( ReadData( pData, NeedLength, &NeedLength ) )
			{
				DispatchPresenterByData( pData );
				bRet = TRUE;
			}
			delete[]pData;
			pData = nullptr;
		}
	}
	return bRet;
}
BOOL CModelData::ReadData(BYTE *pByteBuffer, DWORD dwBufferLength, DWORD *pReturnLength)
{
	if ( HANDLE_SUCCESS( m_hDevice ))
	{
		return ReadFile( m_hDevice, pByteBuffer, dwBufferLength, pReturnLength, nullptr );;
	}
	return FALSE;
}
BOOL CModelData::WriteData(BYTE *pByteBuffer, DWORD dwBufferLength )
{
	DWORD pReturnLength = 0;
	if ( HANDLE_SUCCESS( m_hDevice ))
	{
		return WriteFile( m_hDevice, pByteBuffer, dwBufferLength, &pReturnLength, nullptr );
	}
	return FALSE;
}
BOOL CModelData::DeviceControl( DWORD dwIoControlCode, LPVOID lpBuffer, DWORD BufferSize, LPDWORD lpBytesReturned, BOOL bIn )
{	
	if ( HANDLE_SUCCESS( m_hDevice ))
	{
		if ( bIn )
		{
			return DeviceIoControl( m_hDevice, dwIoControlCode, lpBuffer, BufferSize, nullptr, 0, lpBytesReturned, nullptr );
		}
		else
		{
			return DeviceIoControl( m_hDevice, dwIoControlCode, nullptr, 0, lpBuffer, BufferSize, lpBytesReturned, nullptr );
		}
	}
	
	return FALSE;
}
void CModelData::DispatchPresenterByData( BYTE *pData )
{
	if ( pData != nullptr )
	{
		DataType eType = *( ( DataType* )pData );
		switch ( eType )
		{
		case DATA_TYPE_PROCESS_Create:
		case DATA_TYPE_PROCESS_Exit:
		{
			OutputDebugString( _T( "DATA_TYPE_PROCESSCREATE" ) );
			PPROCESS_DATA pPro = ( PPROCESS_DATA )pData;

			IInterfacePresenter *pPresenter = g_GlobalMap.GetPagePresenter( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour );
			if ( pPresenter != nullptr )
			{
				ProcessCreate *pUser = new ProcessCreate;
				if ( pUser != nullptr )
				{
					pUser->eType = pPro->unit.eType;
					pUser->uParentId = pPro->uParentId;
					pUser->uProceId = pPro->uProceId;
					pUser->uTimes = pPro->uTimes;

					if ( pPro->uCMDLineOffset > 0 )
					{
						pUser->strCmd = (TCHAR *)( pData + pPro->uCMDLineOffset );
					}
					if ( pPro->uParentFileOffset > 0 )
					{
						pUser->strParentFile = ( TCHAR * )( pData + pPro->uParentFileOffset );
					}
					if ( pPro->uProcessFileOffset > 0 )
					{
						pUser->strProcessFile = ( TCHAR * )( pData + pPro->uProcessFileOffset );
					}
					OutputDebugString( pUser->strProcessFile );
					pPresenter->ProcessCreateData( pUser );
				}
			}
		}break;
		case DATA_TYPE_REGISTER_CreateKey:
		case DATA_TYPE_REGISTER_CreateKeyEX:
		case DATA_TYPE_REGISTER_DeleteKey:
		case DATA_TYPE_REGISTER_SetValueKey:
		case DATA_TYPE_REGISTER_DeleteValueKey:
		case DATA_TYPE_REGISTER_QueryKey:
		case DATA_TYPE_REGISTER_QueryValueKey:
		case DATA_TYPE_REGISTER_OpenKey:
		case DATA_TYPE_REGISTER_OpenKeyEx:
		{
				OutputDebugString( _T( "DATA_TYPE_REGISTER_Start" ) );
				PREGISTER_DATA pPro = ( PREGISTER_DATA )pData;
				IInterfacePresenter *pPresenter = g_GlobalMap.GetPagePresenter( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour );
				if ( pPresenter != nullptr )
				{
					RegisterMonitor *pUser = new RegisterMonitor;
					if ( pUser != nullptr )
					{
						pUser->eType = pPro->unit.eType;
						pUser->uProceId = pPro->uProceId;
						pUser->eRegType = pPro->eRegType;
						pUser->uTimes = pPro->uTimes;
						if ( pPro->uRegPathOffset> 0 )
						{
							pUser->strPath = ( TCHAR * )( pData + pPro->uRegPathOffset );
						}
						if ( pPro->uRegNameOffset > 0 )
						{
							pUser->strName = ( TCHAR * )( pData + pPro->uRegNameOffset );
						}
						
						pUser->uDataLength = pPro->uDataLength;
						if ( pPro->uRegDataOffset > 0 )
						{
							pUser->pbuffdata = new BYTE[ pUser->uDataLength +1];
							if ( pUser->pbuffdata != nullptr )
							{
								memset( pUser->pbuffdata, 0, pUser->uDataLength+1 );
								memcpy( pUser->pbuffdata, pData + pPro->uRegDataOffset, pPro->uDataLength );
							}
						}
						else
						{
							pUser->pbuffdata = nullptr;
						}
						OutputDebugString( pUser->strPath );
						pPresenter->ProcessRegisterData( pUser );
					}

				}

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
				OutputDebugString( _T( "DATA_TYPE_FILE_start" ) );
				PFILEOPERATIONMONITOR_DATA pPro = ( PFILEOPERATIONMONITOR_DATA )pData;
				IInterfacePresenter *pPresenter = g_GlobalMap.GetPagePresenter( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour );
				if ( pPresenter != nullptr )
				{
					FileMonitor *pUser = new FileMonitor;
					if ( pUser != nullptr )
					{
						pUser->eType = pPro->unit.eType;
						pUser->uProceId = pPro->uProceId;
						pUser->uFileAttribute = pPro->uFileAttribute;
						pUser->uTimes = pPro->uTimes;
						pUser->eAttType = pPro->eAttType;
						if ( pPro->uFileNameOffset > 0 )
						{
							pUser->strFileName = ( TCHAR * )( pData + pPro->uFileNameOffset );
						}
						pUser->uDatalength = pPro->uDatalength;
						if ( pUser->uDatalength > 0 )
						{
							pUser->pDataBuff = new BYTE[ pUser->uDatalength + 1 ];
							if ( pUser->pDataBuff != nullptr )
							{
								memset( pUser->pDataBuff, 0, pUser->uDatalength + 1 );
								memcpy( pUser->pDataBuff, pData + pPro->uDataOffset, pPro->uDatalength );
							}
						}
						else
						{
							pUser->pDataBuff = nullptr;
						}
						OutputDebugString( pUser->strFileName );
						pPresenter->ProcessFileData( pUser );
					}

				}
		}break;
		case DATA_TYPE_NET_Connect:
		case DATA_TYPE_NET_DNS:
		case DATA_TYPE_NET_Bind:
			{
				OutputDebugString( _T( "DATA_TYPE_NET_start" ) );
				PNETWORKMONITOR_DATA Pro = ( PNETWORKMONITOR_DATA )pData;
				IInterfacePresenter *pPresenter = g_GlobalMap.GetPagePresenter( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour );
				if ( pPresenter != nullptr )
				{
					NetMonitor *pUser = new NetMonitor;
					if ( pUser != nullptr )
					{
						pUser->eType = Pro->unit.eType;
						pUser->uDirection = Pro->uDirection;
						pUser->uProceId = Pro->uProceId;
						pUser->uTimes = Pro->uTimes;
						pUser->uProto = (IPPROTO)Pro->uProto;
						pUser->uDesPort = Pro->uDesPort;
						pUser->uSrcPort = Pro->uSrcPort;
						in_addr addr;
						addr.S_un.S_addr= ntohl(Pro->uSrcIP);
						pUser->SrcIP = Acsi2WideByte( std::string( inet_ntoa( addr ) ) ).c_str( );
						addr.S_un.S_addr = ntohl(Pro->uDesIP);
						pUser->DesIP = Acsi2WideByte(std::string(inet_ntoa( addr ))).c_str();
						pUser->pDataBuff = nullptr;
						if ( Pro->uNetDataOffset > 0 )
						{
							pUser->pDataBuff = new BYTE[ Pro->uDataLength +1];
							if ( pUser->pDataBuff != nullptr )
							{
								memset( pUser->pDataBuff, 0, Pro->uDataLength+1 );
								memcpy( pUser->pDataBuff, pData + Pro->uNetDataOffset, Pro->uDataLength );
							}
						}

						OutputDebugString( pUser->SrcIP );
						pPresenter->ProcessNetData( pUser );

					}
				}
			}break;
		default:
			CString str;
			str.Format( _T( "DispatchPresenterByData %ud" ), *( ( UINT32* )pData ) );
			OutputDebugString( str );
			break;
		}
	}
}
BOOL CModelData::DeviceControl( DWORD IoControlCode, PVOID pData, DataType eType )
{
	BOOL bRet = FALSE;

	switch ( IoControlCode )
	{
	case IOCTL_READ_DATA:
	{
		DWORD dwReturnLength = 0;
		if ( DeviceControl( IOCTL_READ_DATA , nullptr, 0, &dwReturnLength, FALSE )&&( dwReturnLength > 0 ))
		{
			BYTE *pBuffer = new BYTE[ dwReturnLength ];
			if (( pBuffer != nullptr) && DeviceControl( IOCTL_READ_DATA, pBuffer, dwReturnLength, &dwReturnLength, FALSE ))
			{
				DispatchPresenterByData( pBuffer );
				bRet = TRUE;
			}
		}
	}break;
	case IOCTL_WRITE_DATA:
	{
		DWORD dwReturnLength = 0;
		//pData 是用户层数据  可以把数据传化文驱动层数据 在计算长度
		DWORD dwLength = 0;
		BYTE * pBuffer = new BYTE[ dwLength ];
		if ( pBuffer != nullptr )
		{
			bRet = DeviceControl( IOCTL_WRITE_DATA, pBuffer, dwLength, &dwReturnLength, TRUE );
			delete[] pBuffer;
		}
	}break;
	default:
		break;
	}
	
	return bRet;
}
BOOL CModelData::WriteData( PVOID pData )
{
	BOOL bRet = FALSE;
	DWORD dwReturnLength = 0;
	//pData 是用户层数据  可以把数据传化文驱动层数据 在计算长度
	DWORD dwLength = 0;
	BYTE * pBuffer = new BYTE[ dwLength ];
	if ( pBuffer != nullptr )
	{
		bRet = WriteData( pBuffer, dwLength );
		delete[] pBuffer;
	}

	return bRet;
}