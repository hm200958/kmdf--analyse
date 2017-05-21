#pragma once
#include "InterfacePresenter.h"
#include "UserStructDefine.h"

class CProcessBehaviour:public IInterfacePresenter
{
public:
	CProcessBehaviour( );
	virtual ~CProcessBehaviour( );
public:
	virtual void ProcessCreateData( ProcessCreate *pData );
	//×¢²á±í
	virtual void ProcessRegisterData( RegisterMonitor *pData );
	virtual void ProcessNetData( NetMonitor *pData ) ;
	virtual void ProcessFileData( FileMonitor *pData );
	virtual void ReleaseData( );

	virtual void LoadProcessData( UINT32 uProcessId );
protected:
	MULTIMAP_ProcessData *GetProcessData( UINT32  uProcessId );
	void InsertDataToView( DataType eType, PVOID pData );
private:
	MAP_ProcessBehaviourData m_mapProcessBehaviour;
};
