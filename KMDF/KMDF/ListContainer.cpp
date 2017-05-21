
#include "ListContainer.h"
#include "SpinLock.h"

CListContainer::CListContainer()
{
	init();
}
CListContainer::~CListContainer()
{
	Release( );
}

BYTE *CListContainer::GetDataBuffer(ULONG uLength)
{
	BYTE *pData = AllocMemory(uLength);
	if ( pData != nullptr )
	{
		RtlZeroMemory(pData, uLength);
	}
	return pData;
}
BYTE * CListContainer::GetDataFromList(bool bRemove)
{
	CSpinLock spin(&m_SpinLock);

	BYTE *pData = nullptr;
	if ( !IsListEmpty(&m_listHead))
	{
		pData = (BYTE *)RemoveHeadList(&m_listHead);
		if (!bRemove)
		{
			InsertHeadList(&m_listHead, (PLIST_ENTRY)pData);
		}
	}
	return pData;
}
void CListContainer::InsertListData(const BYTE *pData)
{
	CSpinLock spin( &m_SpinLock );
	if ( pData != nullptr )
	{
		InsertTailList(&m_listHead, (PLIST_ENTRY)pData);
	}
}
void CListContainer::FreeDataBuffer( BYTE *pData)
{
	ReleaseMemory(pData);
}
void CListContainer::ClearList()
{
	CSpinLock spin( &m_SpinLock );
	while (!IsListEmpty(&m_listHead))
	{
		BYTE *pData = (BYTE *)RemoveHeadList(&m_listHead);
		ReleaseMemory(pData);
	}
}
void CListContainer::init()
{
	InitializeListHead(&m_listHead);
	KeInitializeSpinLock(&m_SpinLock);
	
}
void CListContainer::Release()
{
	ClearList();
}
BYTE *CListContainer::AllocMemory(ULONG uLength)
{
	BYTE *pData = nullptr;
	if ( uLength > 0 )
	{
		pData = (BYTE*)ExAllocatePoolWithTag(NonPagedPool, uLength,ALLOCTAG);
	}

	return pData;
}
bool CListContainer::ReleaseMemory(BYTE *pData)
{
	if ( pData != nullptr )
	{
		ExFreePoolWithTag(pData,ALLOCTAG);
	}

	return true;
}