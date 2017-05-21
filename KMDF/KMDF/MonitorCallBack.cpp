#include "MonitorCallBack.h"

VOID CreateProcessNotifyEx( __inout PEPROCESS  Process, __in HANDLE  ProcessId, __in_opt PPS_CREATE_NOTIFY_INFO  CreateInfo )
{
	UNREFERENCED_PARAMETER( Process );

	if ( g_CKMDFmanager != nullptr )
	{
		g_CKMDFmanager->ProcessMonitor( ProcessId, CreateInfo );
	}
	else
	{
		KdPrint( ( "CreateProcessNotifyEx:g_CKMDFmanager==NULL\n" ) );
	}
}
NTSTATUS RegistryCallback( IN PVOID CallbackContext, IN PVOID  Argument1, IN PVOID  Argument2 )
{
	KdPrint( ( "RegistryCallback Argument1=%d", Argument1 ) );
	if ( g_CKMDFmanager != nullptr )
	{
		g_CKMDFmanager->RegisterMonitor( REG_NOTIFY_CLASS( ( ULONG )Argument1 ), Argument2 );
	}
	else
	{
		KdPrint( ( "RegistryCallback:g_CKMDFmanager==NULL\n" ) );
	}
	return STATUS_SUCCESS;
}

VOID NetClassifyConnectCallback( const FWPS_INCOMING_VALUES0* inFixedValues,
								 const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
								 PVOID pNetData,const void* classifyContext,
								 const FWPS_FILTER1* filter,UINT64 flowContext,
								 FWPS_CLASSIFY_OUT0* classifyOut)
{
	UNREFERENCED_PARAMETER( classifyContext );
	UNREFERENCED_PARAMETER( flowContext );
	UNREFERENCED_PARAMETER( filter );

 	KdPrint( ( "NetClassifyCallback" ) );
     if ( g_CKMDFmanager != nullptr )
     {
     	g_CKMDFmanager->NetMoniter( inFixedValues, inMetaValues, pNetData, classifyOut,DATA_TYPE_NET_Connect );
     }
     else
     {
 		KdPrint( ( "NetClassifyCallback:g_CKMDFmanager==NULL\n" ) );
     }
 }
    
NTSTATUS NetNotifyConnectCallback( FWPS_CALLOUT_NOTIFY_TYPE notifyType, const GUID* filterKey, FWPS_FILTER1* filter )
{
	 UNREFERENCED_PARAMETER( notifyType );
	 UNREFERENCED_PARAMETER( filterKey );
	 UNREFERENCED_PARAMETER( filter );
   	return STATUS_SUCCESS;
}
   
VOID NetFlowDeleteConnectCallback( IN UINT16 layerId, IN UINT32 calloutId, IN UINT64 flowContext )
{
	UNREFERENCED_PARAMETER( layerId );
	UNREFERENCED_PARAMETER( calloutId );
	UNREFERENCED_PARAMETER( flowContext );
   	//donothing
}
void NetClassifyBindCallback( const FWPS_INCOMING_VALUES0* inFixedValues, 
								 const FWPS_INCOMING_METADATA_VALUES0* inMetaValues, 
								 PVOID pNetData, const void* classifyContext, 
								 const FWPS_FILTER1* filter, UINT64 flowContext, 
								 FWPS_CLASSIFY_OUT0* classifyOut )
{
	UNREFERENCED_PARAMETER( classifyContext );
	UNREFERENCED_PARAMETER( flowContext );
	UNREFERENCED_PARAMETER( filter );

	   KdPrint( ( "NetClassifyCallback" ) );
	   if ( g_CKMDFmanager != nullptr )
	   {
		   g_CKMDFmanager->NetMoniter( inFixedValues, inMetaValues, pNetData, classifyOut, DATA_TYPE_NET_Bind );
	   }
	   else
	   {
		   KdPrint( ( "NetClassifyCallback:g_CKMDFmanager==NULL\n" ) );
	   }
}
NTSTATUS NetNotifyBindCallback( FWPS_CALLOUT_NOTIFY_TYPE notifyType, const GUID* filterKey, FWPS_FILTER1* filter )
{
	UNREFERENCED_PARAMETER( notifyType );
	UNREFERENCED_PARAMETER( filterKey );
	UNREFERENCED_PARAMETER( filter );
	return STATUS_SUCCESS;
}
VOID NetFlowDeleteBindCallback( IN UINT16 layerId, IN UINT32 calloutId, IN UINT64 flowContext )
{
	   UNREFERENCED_PARAMETER( layerId );
	   UNREFERENCED_PARAMETER( calloutId );
	   UNREFERENCED_PARAMETER( flowContext );
	   //donothing
}
FLT_PREOP_CALLBACK_STATUS FilePreCallback( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext )
{
	   if ( g_CKMDFmanager != nullptr )
	   {
		   g_CKMDFmanager->FileMonitor( pData, FltObjects,CompletionContext );
	   }
	   else
	   {
		   KdPrint( ( "FilePreCallback:g_CKMDFmanager==NULL\n" ) );
	   }
	   return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}
FLT_POSTOP_CALLBACK_STATUS FilePostCallback( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags )
{
	UNREFERENCED_PARAMETER( Flags );

	if ( g_CKMDFmanager != nullptr )
	{
		g_CKMDFmanager->FileMonitor( pData, FltObjects, &CompletionContext,false );
	}
	else
	{
		KdPrint( ( "FilePostCallback:g_CKMDFmanager==NULL\n" ) );
	}
	return FLT_POSTOP_FINISHED_PROCESSING;
}
NTSTATUS FileInstanceSetup(PCFLT_RELATED_OBJECTS FltObjects,FLT_INSTANCE_SETUP_FLAGS Flags,DEVICE_TYPE VolumeDeviceType,FLT_FILESYSTEM_TYPE VolumeFilesystemType )
{
	   UNREFERENCED_PARAMETER( FltObjects );
	   UNREFERENCED_PARAMETER( Flags );
	   UNREFERENCED_PARAMETER( VolumeDeviceType );
	   UNREFERENCED_PARAMETER( VolumeFilesystemType );

	   return STATUS_SUCCESS;
}
NTSTATUS FileInstanceQueryTeardown(PCFLT_RELATED_OBJECTS FltObjects,FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	  UNREFERENCED_PARAMETER( FltObjects );
	  UNREFERENCED_PARAMETER( Flags );

	  return STATUS_SUCCESS;
}
VOID FileInstanceTeardownStart(PCFLT_RELATED_OBJECTS FltObjects,FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	   UNREFERENCED_PARAMETER( FltObjects );
	   UNREFERENCED_PARAMETER( Flags );
}
VOID NPInstanceTeardownComplete(PCFLT_RELATED_OBJECTS FltObjects,FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	   UNREFERENCED_PARAMETER( FltObjects );
	   UNREFERENCED_PARAMETER( Flags );
}
NTSTATUS FileUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER( Flags );
	return STATUS_SUCCESS;
}
