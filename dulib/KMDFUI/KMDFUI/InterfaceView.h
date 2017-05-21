#pragma once
#include "UserDataDefine.h"
interface IInterfaceView
{
public:
	IInterfaceView() {};
	virtual ~IInterfaceView() {};
public:
	virtual void InsertProcessTree( ProcessCreate *pData ) {};
	virtual void InsertListProcessData( ProcessCreate *pData ) {};
	virtual void InsertListRegisterData( RegisterMonitor *pData ) {};
	virtual void InsertListNetData( NetMonitor *pData ) {};
	virtual void InsertListFileData( FileMonitor *pData ) {};

	virtual bool CheckCurProcessSel( UINT32 uPid ) { return false; };
};