#include "StdAfx.h"
#include "NetWFPFilter.h"
CNetWFPFilter::CNetWFPFilter()
{
	RtlZeroMemory( &m_stCallbackID, sizeof( m_stCallbackID ) );
	m_hEngine = nullptr;
}
CNetWFPFilter::~CNetWFPFilter( )
{

}
bool CNetWFPFilter::init( )
{
	if ( OpenWfpEngine( ) )
	{
		FwpmTransactionBegin( m_hEngine, 0 );
		InitConnectWfp( );
		InitBindWfp( );
		FwpmTransactionCommit( m_hEngine );
	}

	return false;
}

 void CNetWFPFilter::InitConnectWfp( )
 {
 		if ( AddCallOutToEngine( NETCONNECTNAME , NETCONNECTDES, CallOutTCPUDPConnect_V4, FWPM_LAYER_ALE_AUTH_CONNECT_V4, &m_stCallbackID[ 0 ].uCallOutAddID ))
 		{
 			if ( AddSubLayerToEngine( NETCONNECTSUBLAYERNAME ,NETCONNECTSUBLAYERDES, CallOutTCPUDPConnectSublayer_V4 ,(UINT16)546780))
 			{
 				m_stCallbackID[ 0 ].pLayerAddID = &CallOutTCPUDPConnectSublayer_V4;
 
 				FWP_V4_ADDR_AND_MASK addrmsk = { 0 };
 				FWPM_FILTER_CONDITION con[ 1 ] = { 0 };
 				con[ 0 ].fieldKey = FWPM_CONDITION_IP_REMOTE_ADDRESS;
 				con[ 0 ].matchType = FWP_MATCH_EQUAL;
 				con[ 0 ].conditionValue.type = FWP_V4_ADDR_MASK;
 				con[ 0 ].conditionValue.v4AddrMask = &addrmsk;
 
 				AddFitrerToEngine( NETCONNECTFILTERNAME, NETCONNECTFILTERDES, CallOutTCPUDPConnect_V4, FWPM_LAYER_ALE_AUTH_CONNECT_V4, CallOutTCPUDPConnectSublayer_V4, 0, con, &m_stCallbackID[ 0 ].uFilterAddID );
 			}
 		}
 }
 void CNetWFPFilter::InitBindWfp( )
 {
 
 		if ( AddCallOutToEngine( NETBINDNAME, NETBINDDES, CallOutTCPUDPBind_V4 , FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4 ,&m_stCallbackID[1].uCallOutAddID ))
 		{
 			if ( AddSubLayerToEngine( NETBINDSUBLAYERNAME, NETBINDSUBLAYERDES , CallOutTCPUDPBindSublayer_V4 , (UINT16)546781))
 			{
 				m_stCallbackID[ 1 ].pLayerAddID = &CallOutTCPUDPBindSublayer_V4;
 
 				FWP_V4_ADDR_AND_MASK addrmsk = { 0 };
 				FWPM_FILTER_CONDITION con[ 1 ] = { 0 };
 				con[ 0 ].fieldKey = FWPM_CONDITION_IP_LOCAL_ADDRESS_TYPE;
 				con[ 0 ].matchType = FWP_MATCH_EQUAL;
 				con[ 0 ].conditionValue.type = FWP_V4_ADDR_MASK;
 				con[ 0 ].conditionValue.v4AddrMask = &addrmsk;
 
 				AddFitrerToEngine( NETBINDFILTERNAME, NETBINDFILTERDES, CallOutTCPUDPBind_V4, FWPM_LAYER_ALE_RESOURCE_ASSIGNMENT_V4, CallOutTCPUDPBindSublayer_V4, 1, con, &m_stCallbackID[ 1 ].uFilterAddID );
 			}
 		}
 	
 }
void CNetWFPFilter::Release( )
{
	ReleaseWfp( );
}
 bool CNetWFPFilter::OpenWfpEngine( )
 {
 	FWPM_SESSION0 Session = { 0 };
 	if ( FwpmEngineOpen( NULL, RPC_C_AUTHN_WINNT, NULL, &Session, &m_hEngine ) == NO_ERROR)
 	{
 		return true;
 	}
 	
 	return false;
 }
 bool CNetWFPFilter::AddCallOutToEngine( PTCHAR pwCharName, PTCHAR pwCharDes,const GUID &CallGuid,const GUID &LayerGuid, UINT32 *pAddCallOutID )
 {
 	if ( HANDLE_SUCCESS( m_hEngine ) && ( pAddCallOutID != nullptr ) && ( CallGuid != GUID{ 0 }) && ( LayerGuid != GUID{ 0 } ))
 	{
 		FWPM_CALLOUT fwpmCallout;
 		RtlZeroMemory( &fwpmCallout, sizeof( FWPM_CALLOUT ) );
 		fwpmCallout.flags = 0;
 
 		fwpmCallout.displayData.name = pwCharName;
 		fwpmCallout.displayData.description = pwCharDes;
 		fwpmCallout.calloutKey = CallGuid;
 		fwpmCallout.applicableLayer = LayerGuid;
 		HRESULT status = FwpmCalloutAdd( m_hEngine, &fwpmCallout, NULL, pAddCallOutID );
 		if ( status == S_OK )
 		{
 			return true;
 		}
 
 	}
 	return false;
 }
 bool CNetWFPFilter::AddSubLayerToEngine( PTCHAR pwCharName, PTCHAR pwCharDes,const GUID &subLayerGuid, UINT16 uWeight )
 {
 	if ( HANDLE_SUCCESS( m_hEngine ) && ( subLayerGuid != GUID{ 0 } ) )
 	{
 		FWPM_SUBLAYER subLayer;
 		RtlZeroMemory( &subLayer, sizeof( FWPM_SUBLAYER ) );
 		subLayer.displayData.name = pwCharName;
 		subLayer.displayData.description = pwCharDes;
 		subLayer.flags = 0;
 		subLayer.subLayerKey = subLayerGuid;
 		subLayer.weight = uWeight;
 		if ( S_OK == FwpmSubLayerAdd(m_hEngine,&subLayer,nullptr ) )
 		{
 			return true;
 		}
 	}
 
 	return false;
 }
 bool CNetWFPFilter::AddFitrerToEngine( PTCHAR pwCharName, PTCHAR pwCharDes, const GUID &callGuid, const GUID &bindLayerGuid, const GUID &subLayerGuid, UINT32 uNumFilter, FWPM_FILTER_CONDITION *filterCondition, UINT64 *pAddFilterID )
 {
 	if ( HANDLE_SUCCESS( m_hEngine ) && ( callGuid != GUID{ 0 } ) && ( bindLayerGuid != GUID{ 0 } ) && ( subLayerGuid != GUID{ 0 } ) )
 	{
 		FWPM_FILTER filter;
 		RtlZeroMemory( &filter, sizeof( FWPM_FILTER ) );
 
 		filter.displayData.name = pwCharName;
 		filter.displayData.description = pwCharDes;
 
 		filter.flags = 0;
 		filter.layerKey = bindLayerGuid;
 		filter.subLayerKey = subLayerGuid;
 		filter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
 		filter.action.calloutKey = callGuid;
 
 		filter.numFilterConditions = uNumFilter;
 		filter.filterCondition = filterCondition;
 
 		filter.weight.type = FWP_EMPTY;
 		if ( S_OK == FwpmFilterAdd( m_hEngine,&filter,nullptr, pAddFilterID ))
 		{
 			return true;
 		}
 	}
 
 	return false;
 }
void CNetWFPFilter::ReleaseWfp( )
{
	if ( HANDLE_SUCCESS( m_hEngine ))
	{
		for ( UINT32 i = 0; i < NetMonitorCallbackIDNum; ++i )
		{
		 	FwpmFilterDeleteById( m_hEngine, m_stCallbackID[i].uFilterAddID );
		 	if ( m_stCallbackID[ i ].pLayerAddID != nullptr  )
		 	{
		 		FwpmSubLayerDeleteByKey( m_hEngine, m_stCallbackID[ i ].pLayerAddID );
		 	}
		 	FwpmCalloutDeleteById( m_hEngine, m_stCallbackID[ i ].uCallOutAddID );
		}

		RtlZeroMemory( &m_stCallbackID, sizeof( m_stCallbackID ) );

		FwpmEngineClose( m_hEngine );
		m_hEngine = nullptr;
	}
}