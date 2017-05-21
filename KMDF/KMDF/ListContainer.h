#pragma once

#include "DriverDataDefine.h"
#include "DriverOperator.h"

class CListContainer:public CDriverMem
{
public:
	CListContainer();
	virtual ~CListContainer();

public:
	BYTE *GetDataBuffer(ULONG uLength);
	void FreeDataBuffer(BYTE *pData);
	BYTE *GetDataFromList(bool bRemove = true);
	void InsertListData(const BYTE *pData);
	void ClearList();
protected:
	void init();
	void Release();

	BYTE *AllocMemory(ULONG uLength);
	bool ReleaseMemory( BYTE *pData);
private:
	LIST_ENTRY m_listHead;
	KSPIN_LOCK m_SpinLock;
};
