#pragma once
#include "ObjectInterface.h"
#include "ProtocolAnalysis.h"
class CNetMonitor:public CObjectInterface
{
public:
	CNetMonitor( CListContainer *pList );
	virtual ~CNetMonitor( );


	virtual bool init( );
	virtual void Release( );

	virtual void NetMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData, OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType );
protected:
	void InitConnectWfp( );
	void InitBindWfp( );
	void ReleaseWfp( );
	bool RegisterCallOutCallBack(const GUID &CallUid, FWPS_CALLOUT_CLASSIFY_FN pCLassFy, FWPS_CALLOUT_NOTIFY_FN pNotify, FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN pDelete, UINT32 *pCallId );
protected:
	void ConnectMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues );
	void BindMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues );
	void DNSMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData );
	UINT32 GetNetBufferListLength( PNET_BUFFER_LIST pNetList );
	void GetNetBufferListData( PNET_BUFFER_LIST pNetList, BYTE *&pBuffer );


private:
	UINT32 m_stCallbackID[ NetMonitorCallbackIDNum ];
	CProtocolAnalysis m_objProtocol;
};
