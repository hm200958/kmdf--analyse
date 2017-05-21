#pragma once
#include "DriverOperator.h"
#include "DriverDataDefine.h"
#include "ListContainer.h"


class CObjectInterface:public CDriverMem
{
public:
	CObjectInterface(CListContainer *pList);
	virtual ~CObjectInterface( );

public:
	virtual bool init( );
	virtual void Release( );
	virtual bool InsertDataList( const BYTE *pData );
	virtual BYTE *GetDataBuffer( UINT32 nLength );
	virtual void FreeDataBuffer( BYTE *pData );
	virtual void ProcessMonitorData(const PVOID handle, const PVOID pCreateinfo);//进程创建退出行为

	virtual void RegisterMonitorData( REG_NOTIFY_CLASS regType, const PVOID pData );//注册表
	
	virtual void NetMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData, OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType );//网络
	virtual void FileMonitorData( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, bool bPre );//文件

protected:
	CListContainer *m_pListContainer;
};
