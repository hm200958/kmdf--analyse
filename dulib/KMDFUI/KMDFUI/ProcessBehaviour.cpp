#include "StdAfx.h"
#include "ProcessBehaviour.h"

CProcessBehaviour::CProcessBehaviour( )
{
	m_mapProcessBehaviour.clear( );
}
CProcessBehaviour::~CProcessBehaviour( )
{

}
void CProcessBehaviour::ProcessCreateData( ProcessCreate *pData )
{
	if (( pData != nullptr )&&( m_pInterView != nullptr ))
	{
		auto it = m_mapProcessBehaviour.find( pData->uParentId ); //插入父进程行为数据里 
		if ( it != m_mapProcessBehaviour.end( ) )
		{
			it->second.insert( std::make_pair( pData->eType, pData ) );
		}

		it = m_mapProcessBehaviour.find( pData->uProceId );
		if ( it != m_mapProcessBehaviour.end( ))
		{
			it->second.insert( std::make_pair( pData->eType, pData ));
		}
		else
		{
			MULTIMAP_ProcessData data;
			data.insert( std::make_pair( pData->eType, pData ) );
			m_mapProcessBehaviour.insert( std::make_pair( pData->uProceId, data ));
		}

		m_pInterView->InsertProcessTree( pData );
	}
}
void CProcessBehaviour::ProcessRegisterData( RegisterMonitor *pData )
{
	if (( pData != nullptr )&&( m_pInterView != nullptr ))
	{
		auto it = m_mapProcessBehaviour.find( pData->uProceId );
		if ( it != m_mapProcessBehaviour.end( ) )
		{
			it->second.insert( std::make_pair( pData->eType, pData ) );
			if ( m_pInterView->CheckCurProcessSel( pData->uProceId ))
			{
				m_pInterView->InsertListRegisterData( pData );
			}
			
		}
		else
		{
			if ( pData->pbuffdata != nullptr )
			{
				delete[]pData->pbuffdata;
				pData->pbuffdata = nullptr;
			}
			delete pData;
		}
	}
}
void CProcessBehaviour::ProcessNetData( NetMonitor *pData )
{
	if ( ( pData != nullptr ) && ( m_pInterView != nullptr ) )
	{
		auto it = m_mapProcessBehaviour.find( pData->uProceId ); 
		if ( it != m_mapProcessBehaviour.end( ) )
		{
			it->second.insert( std::make_pair( pData->eType, pData ) );
			if ( m_pInterView->CheckCurProcessSel( pData->uProceId ) )
			{
				m_pInterView->InsertListNetData( pData );
			}

		}
		else
		{
			if ( pData->pDataBuff != nullptr )
			{
				delete[]pData->pDataBuff;
				pData->pDataBuff = nullptr;
			}
			delete pData;
		}
	}
}
void CProcessBehaviour::ProcessFileData( FileMonitor *pData )
{
	if ( ( pData != nullptr ) && ( m_pInterView != nullptr ) )
	{
		auto it = m_mapProcessBehaviour.find( pData->uProceId );
		if ( it != m_mapProcessBehaviour.end( ) )
		{
			it->second.insert( std::make_pair( pData->eType, pData ) );
			if ( m_pInterView->CheckCurProcessSel( pData->uProceId ) )
			{
				m_pInterView->InsertListFileData( pData );
			}

		}
		else
		{
			if ( pData->pDataBuff != nullptr )
			{
				delete[]pData->pDataBuff;
				pData->pDataBuff = nullptr;
			}
			delete pData;
		}
	}
}
void CProcessBehaviour::LoadProcessData( UINT32 uProcessId )
{
	if (  uProcessId > 0 )
	{
		MULTIMAP_ProcessData * pData = GetProcessData( uProcessId );
		if ( pData != nullptr )
		{
			auto it = pData->begin( );
			while ( it != pData->end(  ))
			{
				InsertDataToView( it->first, it->second );
				++it;
			}
		}
	}
}
MULTIMAP_ProcessData *CProcessBehaviour::GetProcessData( UINT32  uProcessId )
{
	auto it = m_mapProcessBehaviour.find( uProcessId ); //插入父进程行为数据里 
	if ( it != m_mapProcessBehaviour.end( ) )
	{
		return &(it->second);
	}

	return nullptr;
}
void CProcessBehaviour::InsertDataToView( DataType eType, PVOID pData )
{
	if ( m_pInterView != nullptr && pData != nullptr )
	{
		switch ( eType )
		{
		case DATA_TYPE_PROCESS_Exit:
		case DATA_TYPE_PROCESS_Create:
		{
			m_pInterView->InsertListProcessData( ( ProcessCreate * )pData );

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
				m_pInterView->InsertListRegisterData( ( RegisterMonitor * )pData );
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
				m_pInterView->InsertListFileData( ( FileMonitor* )pData );
		}break;
		case DATA_TYPE_NET_Bind:
		case DATA_TYPE_NET_Connect:
		case DATA_TYPE_NET_DNS:
			{
				m_pInterView->InsertListNetData( (NetMonitor *)pData );
			}break;
		default:
			break;
		}
	}
}
void CProcessBehaviour::ReleaseData( )
{
	auto it = m_mapProcessBehaviour.begin( );
	while ( it != m_mapProcessBehaviour.end( ))
	{
		auto it2 = it->second.begin( );
		while ( it2 != it->second.end( ))
		{
			if (( it2->first > DATA_TYPE_REGISTER_START )&& ( it2->first <= DATA_TYPE_REGISTER_END ))
			{
				if ( ((RegisterMonitor *)(it2->second))->pbuffdata != nullptr )
				{
					delete[]( ( RegisterMonitor * )( it2->second ) )->pbuffdata;
					( ( RegisterMonitor * )( it2->second ) )->pbuffdata = nullptr;
				}
				delete ( RegisterMonitor * )it2->second;
			}
			else if ( ( it2->first > DATA_TYPE_FILE_START ) && ( it2->first < DATA_TYPE_FILE_END ) )
			{
				if ( ( ( FileMonitor * )( it2->second ) )->pDataBuff != nullptr )
				{
					delete[]( ( FileMonitor * )( it2->second ) )->pDataBuff;
					( ( FileMonitor * )( it2->second ) )->pDataBuff = nullptr;
				}
				delete ( FileMonitor * )it2->second;
			}
			else if ( ( it2->first > DATA_TYPE_NET_START ) && ( it2->first < DATA_TYPE_NET_END ) )
			{
				if ( ( ( NetMonitor * )( it2->second ) )->pDataBuff != nullptr )
				{
					delete[]( ( NetMonitor * )( it2->second ) )->pDataBuff;
					( ( NetMonitor * )( it2->second ) )->pDataBuff = nullptr;
				}
				delete ( NetMonitor * )it2->second;
			}
			else if ( !( ( it2->first == DATA_TYPE_PROCESS_Create ) && ( it->first == ( ( ProcessCreate * )( it2->second ) )->uParentId ) ) ) 
			{
			    delete ( ProcessCreate * )it2->second;
			}
			
			++it2;
		}

		it->second.clear( );
		++it;
	}

	m_mapProcessBehaviour.clear( );
}