#include "NetMonitor.h"
#include "MonitorCallBack.h"

CNetMonitor::CNetMonitor( CListContainer *pList ):CObjectInterface( pList )
{
	RtlZeroMemory( &m_stCallbackID, sizeof( m_stCallbackID ) );
}
CNetMonitor::~CNetMonitor( )
{

}
bool CNetMonitor::init( )
{
	InitConnectWfp( );
	InitBindWfp( );
	
	return true;
}

 void CNetMonitor::InitConnectWfp( )
 {
	 RegisterCallOutCallBack( CallOutTCPUDPConnect_V4, NetClassifyConnectCallback, NetNotifyConnectCallback, NetFlowDeleteConnectCallback, &m_stCallbackID[ 0 ] );
 	
 }
 void CNetMonitor::InitBindWfp( )
 {
	 RegisterCallOutCallBack( CallOutTCPUDPBind_V4, NetClassifyBindCallback, NetNotifyBindCallback, NetFlowDeleteBindCallback, &m_stCallbackID[ 1 ] );
 }
void CNetMonitor::Release( )
{
	ReleaseWfp( );
	__super::Release( );
}
void CNetMonitor::NetMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData, OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType )
{
	if (( inFixedValues != nullptr ) && ( inMetaValues != nullptr ) && ( classifyOut != nullptr ))
	{
		///inFixedValues->layerId  FWPS_BUILTIN_LAYERS
		switch ( eType )
		{
			case DATA_TYPE_NET_DNS:
			case DATA_TYPE_NET_Connect:
				{
					IPPROTO ePro = (IPPROTO)inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL ].value.uint8;
					UINT16 uDesPort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT ].value.uint16;
					if (( uDesPort == DNSPORT ) && ( ePro == IPPROTO_UDP ))
					{
						DNSMonitorData( inFixedValues, inMetaValues, pNetrData );
					}
					else
					{
						ConnectMonitorData( inFixedValues, inMetaValues );
					}
					
				}break;
			case DATA_TYPE_NET_Bind:
				{
					BindMonitorData( inFixedValues, inMetaValues );
				}break;
			default:
				break;
		}

		classifyOut->actionType = FWP_ACTION_PERMIT;
	}
	//WORD	wDirection = 0;
	//WORD	wRemotePort = 0;
	//WORD	wSrcPort = 0;
	//
	//ULONG	ulSrcIPAddress = 0;
	//ULONG	ulRemoteIPAddress = 0;
	////wDirection表示数据包的方向,取值为	//FWP_DIRECTION_INBOUND/FWP_DIRECTION_OUTBOUND
	//wDirection = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_DIRECTION ].value.int8;

	////wSrcPort表示本地端口，主机序
	//wSrcPort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_PORT ].value.uint16;

	////wRemotePort表示远端端口，主机序
	//wRemotePort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_PORT ].value.uint16;

	////ulSrcIPAddress 表示源IP
	//ulSrcIPAddress = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_LOCAL_ADDRESS ].value.uint32;

	////ulRemoteIPAddress 表示远端IP
	//ulRemoteIPAddress = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_REMOTE_ADDRESS ].value.uint32;

	////wProtocol表示网络协议，可以取值是IPPROTO_ICMP/IPPROTO_UDP/IPPROTO_TCP
	//WORD wProtocol = inFixedValues->incomingValue[ FWPS_FIELD_ALE_FLOW_ESTABLISHED_V4_IP_PROTOCOL ].value.uint8;

	////默认"允许"(PERMIT)
	//


	//if ( eType == DATA_TYPE_NET_Connect )
	//{
	//	
	//}
	//else
	//{
	//	KdPrint( ( "DATA_TYPE_NET_Bind: Bind=%d,ulSrcIPAddress=%d, wSrcPort=%d,ulRemoteIPAddress=%d,wRemotePort=%d\n", wProtocol, ulSrcIPAddress, wSrcPort, ulRemoteIPAddress, wRemotePort ) );
	//}
	//
	////简单的策略判断，读者可以重写这部分
	//// 	if( (wProtocol == IPPROTO_TCP) && 
	//// 		(wDirection == FWP_DIRECTION_OUTBOUND) && 
	//// 		(wRemotePort == HTTP_DEFAULT_PORT) )
	//// 	{
	//// 		//TCP协议尝试发起80端口的访问，拦截(BLOCK)
	//// 		classifyOut->actionType = FWP_ACTION_BLOCK;
	//// 	}

	////清除FWPS_RIGHT_ACTION_WRITE标记
}
bool CNetMonitor::RegisterCallOutCallBack( const GUID &CallUid, FWPS_CALLOUT_CLASSIFY_FN pCLassFy, FWPS_CALLOUT_NOTIFY_FN pNotify, FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN pDelete,UINT32 *pCallId )
{
	if ( (CallUid != GUID{ 0 }) && ( pCLassFy != nullptr ) )
	{
		FWPS_CALLOUT swfpCallout;
		RtlZeroMemory( &swfpCallout, sizeof( FWPS_CALLOUT ) );

		swfpCallout.calloutKey = CallUid;
		swfpCallout.flags = 0;
		swfpCallout.classifyFn = pCLassFy;
		swfpCallout.notifyFn = pNotify;
		swfpCallout.flowDeleteFn = pDelete;
		NTSTATUS status = FwpsCalloutRegister( g_DvriverControlObject, &swfpCallout, pCallId );
		if ( NT_SUCCESS( status ) ||( status == STATUS_FWP_ALREADY_EXISTS))
		{
			return true;
		}
		
	}
	return FALSE;
}
void CNetMonitor::ReleaseWfp( )
{
 	for ( UINT32 i = 0; i < NetMonitorCallbackIDNum; ++i )
 	{
 		FwpsCalloutUnregisterById( m_stCallbackID[ i ] );
	}

	RtlZeroMemory( &m_stCallbackID, sizeof( m_stCallbackID ) );
}
void CNetMonitor::ConnectMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues )
{
	if ( ( inFixedValues != nullptr ) && ( inMetaValues != nullptr ) )
	{
		PNETWORKMONITOR_DATA pBuffer = ( PNETWORKMONITOR_DATA )GetDataBuffer( sizeof( NETWORKMONITOR_DATA ) );
		if ( pBuffer != nullptr )
		{
			pBuffer->unit.eType = DATA_TYPE_NET_Connect;
			pBuffer->unit.nLength = sizeof( NETWORKMONITOR_DATA );
			pBuffer->uDirection = FWP_DIRECTION_OUTBOUND;
			pBuffer->uProto = ( IPPROTO )inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL ].value.uint8;
			if ( FWPS_IS_METADATA_FIELD_PRESENT( inMetaValues ,FWPS_METADATA_FIELD_PROCESS_ID ) )
			{
				pBuffer->uProceId = ( UINT32 )inMetaValues->processId;
			}
			//t表示本地端口，主机序
			pBuffer->uSrcPort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT ].value.uint16;
			//表示远端端口，主机序
			pBuffer->uDesPort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT ].value.uint16;
			// 表示源IP
			pBuffer->uSrcIP = inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS_TYPE ].value.uint32;
			//表示远端IP
			pBuffer->uDesIP = inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS ].value.uint32;

			KdPrint( ( "DATA_TYPE_NET_Connect: PID=%d,connect=%d,ulSrcIPAddress=%d, wSrcPort=%d,ulRemoteIPAddress=%d,wRemotePort=%d\n", pBuffer->uProceId, pBuffer->uProto, pBuffer->uSrcIP, pBuffer->uSrcPort, pBuffer->uDesIP, pBuffer->uDesPort ) );
			InsertDataList( ( BYTE* )pBuffer );
		}
	}
}
void CNetMonitor::BindMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues )
{
	if ( ( inFixedValues != nullptr ) && ( inMetaValues != nullptr ) )
	{
		PNETWORKMONITOR_DATA pBuffer = ( PNETWORKMONITOR_DATA )GetDataBuffer( sizeof( NETWORKMONITOR_DATA ) );
		if ( pBuffer != nullptr )
		{
			pBuffer->unit.eType = DATA_TYPE_NET_Bind;
			pBuffer->unit.nLength = sizeof( NETWORKMONITOR_DATA );
			pBuffer->uDirection = FWP_DIRECTION_MAX;
			pBuffer->uProto = ( IPPROTO )inFixedValues->incomingValue[ FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_PROTOCOL ].value.uint8;
			if ( FWPS_IS_METADATA_FIELD_PRESENT( inMetaValues, FWPS_METADATA_FIELD_PROCESS_ID ) )
			{
				pBuffer->uProceId = ( UINT32 )inMetaValues->processId;
			}
			//t表示本地端口，主机序
			pBuffer->uSrcPort = inFixedValues->incomingValue[ FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_PORT ].value.uint16;
			pBuffer->uSrcIP = inFixedValues->incomingValue[ FWPS_FIELD_ALE_RESOURCE_ASSIGNMENT_V4_IP_LOCAL_ADDRESS ].value.uint32;
			KdPrint( ( "DATA_TYPE_NET_Bind: PID=%d,Bind=%d,ulSrcIPAddress=%d, wSrcPort=%d\n", pBuffer->uProceId, pBuffer->uProto, pBuffer->uSrcIP, pBuffer->uSrcPort) );
			InsertDataList( ( BYTE* )pBuffer );
		}
	}
}
void CNetMonitor::DNSMonitorData( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues, PVOID pNetrData )
{
	if ( ( inFixedValues != nullptr ) && ( inMetaValues != nullptr ) && ( pNetrData != nullptr ))
	{
		UINT32 uTotal = sizeof( NETWORKMONITOR_DATA );
		UINT32 NetLentgh = GetNetBufferListLength( PNET_BUFFER_LIST( pNetrData ) );
		if ( NetLentgh > 0 )
		{
			
			BYTE * pNetBuff = GetDataBuffer( NetLentgh + 2 );
			if ( pNetBuff != nullptr )
			{
				BYTE *pDns = nullptr;

				GetNetBufferListData( PNET_BUFFER_LIST( pNetrData ), pNetBuff );
				if ( m_objProtocol.GetDnsNameData( pNetBuff, pDns,NetLentgh ) &&( NetLentgh > 0 ))
				{
					uTotal += NetLentgh+1;

					PNETWORKMONITOR_DATA pBuffer = ( PNETWORKMONITOR_DATA )GetDataBuffer( uTotal );
					if ( pBuffer != nullptr )
					{
						pBuffer->unit.eType = DATA_TYPE_NET_DNS;
						pBuffer->unit.nLength = uTotal;
						pBuffer->uDataLength = NetLentgh;
						pBuffer->uNetDataOffset = sizeof( NETWORKMONITOR_DATA );
						pBuffer->uDirection = FWP_DIRECTION_OUTBOUND;
						pBuffer->uProto = ( IPPROTO )inFixedValues->incomingValue[ FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL ].value.uint8;
						if ( FWPS_IS_METADATA_FIELD_PRESENT( inMetaValues, FWPS_METADATA_FIELD_PROCESS_ID ) )
						{
							pBuffer->uProceId = ( UINT32 )inMetaValues->processId;
						}
						RtlCopyMemory( ( ( BYTE * )pBuffer + pBuffer->uNetDataOffset ), pDns, NetLentgh );
						KdPrint( ( "DATA_TYPE_NET_Connect: PID=%d,connect=%d, wSrcPort=%d,ulRemoteIPAddress=%d,wRemotePort=%d\n", pBuffer->uProceId, pBuffer->uProto, pBuffer->uSrcPort, pBuffer->uDesIP, pBuffer->uDesPort ) );
						InsertDataList( ( BYTE* )pBuffer );
					}
				}

				FreeDataBuffer( pNetBuff );
			}
		}


	}
	else
	{
		if ( pNetrData == nullptr )
		{
			KdPrint( ( "DATA_TYPE_NET_DNS pnetdata is nullptr\n" ) );
		}
	}
}
UINT32 CNetMonitor::GetNetBufferListLength( PNET_BUFFER_LIST pNetList )
{
	UINT32 nDataLength = 0;
	PNET_BUFFER pNetBuffer = nullptr;
	if ( pNetList != nullptr )
	{
		do //MmGetSystemAddressForMdlSafe  NdisQueryMdl
		{
			pNetBuffer = NET_BUFFER_LIST_FIRST_NB( pNetList );
			do
			{
				nDataLength += NET_BUFFER_DATA_LENGTH( pNetBuffer );
				pNetBuffer = NET_BUFFER_NEXT_NB( pNetBuffer );
			} while ( pNetBuffer != nullptr );

			pNetList = NET_BUFFER_LIST_NEXT_NBL( pNetList );
		} while ( pNetList != nullptr );
	}
	return nDataLength;
}
void CNetMonitor::GetNetBufferListData( PNET_BUFFER_LIST pNetList, BYTE * &pBuffer )
{
	ULONG uDataWrite = 0;
	if ( ( pNetList != nullptr ) && ( pBuffer != nullptr ) )
	{
		do
		{
			PNET_BUFFER pNetBuffer = NET_BUFFER_LIST_FIRST_NB( pNetList );
			do
			{
				BYTE *pHeader = nullptr;
				ULONG uOffset = NET_BUFFER_DATA_OFFSET( pNetBuffer );
				PMDL mdl = NET_BUFFER_FIRST_MDL( pNetBuffer );
				ULONG uLength = NET_BUFFER_DATA_LENGTH( pNetBuffer );
				do
				{
					NdisQueryMdl( mdl, &pHeader, &uLength, NormalPagePriority );
					if (( pHeader != nullptr) && ( uLength >0 ))
					{
						if ( uLength > uOffset )
						{
							RtlCopyMemory( pBuffer + uDataWrite, pHeader + uOffset, uLength - uOffset );
							uDataWrite += uLength - uOffset;
							uOffset = 0;
						}
						else
						{
							uOffset -= uLength;
							continue;
						}
					}
					mdl = mdl->Next;
				} while ( mdl != nullptr );

				pNetBuffer = NET_BUFFER_NEXT_NB( pNetBuffer );
			} while ( pNetBuffer != nullptr );

			pNetList = NET_BUFFER_LIST_NEXT_NBL( pNetList );
		} while ( pNetList != nullptr );
	}
}