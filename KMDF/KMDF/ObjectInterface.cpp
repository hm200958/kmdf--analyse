#include "ObjectInterface.h"

CObjectInterface::CObjectInterface(CListContainer *pList):m_pListContainer(pList)
{

}
CObjectInterface::~CObjectInterface( )
{
	Release( );
}
bool CObjectInterface::init( )
{
	return false;
}
void CObjectInterface::Release()
{
	m_pListContainer = nullptr;
}
bool CObjectInterface::InsertDataList( const BYTE *pData )
{
	if ( m_pListContainer != nullptr && pData != nullptr  )
	{
		m_pListContainer->InsertListData( pData );
		
		return true;
	}
	return false;
}
BYTE *CObjectInterface::GetDataBuffer( UINT32 nLength )
{
	BYTE *pBuffer = nullptr;
	if ( m_pListContainer != nullptr )
	{
		pBuffer = m_pListContainer->GetDataBuffer( nLength );
	}

	return pBuffer;
}
void CObjectInterface::FreeDataBuffer( BYTE *pData )
{
	if ( m_pListContainer != nullptr )
	{
		m_pListContainer->FreeDataBuffer( pData );
	}
}
void CObjectInterface::ProcessMonitorData(const PVOID handle, const PVOID pCreateinfo)
{

}
void CObjectInterface::RegisterMonitorData( REG_NOTIFY_CLASS regType, const PVOID pData )
{

}
void CObjectInterface::NetMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData, OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType )
{

}
void CObjectInterface::FileMonitorData( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, bool bPre )
{

}