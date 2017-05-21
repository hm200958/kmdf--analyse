#pragma once
#include "KmdfManager.h"
EXTERN_C_START
//进程
VOID CreateProcessNotifyEx( __inout PEPROCESS  Process, __in HANDLE  ProcessId, __in_opt PPS_CREATE_NOTIFY_INFO  CreateInfo );
//注册表
NTSTATUS RegistryCallback( IN PVOID CallbackContext, IN PVOID  Argument1, IN PVOID  Argument2 );
//网络连接
VOID NetClassifyConnectCallback( const FWPS_INCOMING_VALUES0* inFixedValues,
								 const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
								 PVOID pNetData,
								 const void* classifyContext,
								 const FWPS_FILTER1* filter,
								 UINT64 flowContext,
								 FWPS_CLASSIFY_OUT0* classifyOut );
NTSTATUS NetNotifyConnectCallback( FWPS_CALLOUT_NOTIFY_TYPE notifyType,
								   const GUID* filterKey,
								   FWPS_FILTER1* filter);
VOID NetFlowDeleteConnectCallback( IN UINT16 layerId, IN UINT32 calloutId, IN UINT64 flowContext );
//网络绑定
VOID NetClassifyBindCallback( const FWPS_INCOMING_VALUES0* inFixedValues,
							  const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
							  PVOID pNetData, const void* classifyContext,
							  const FWPS_FILTER1* filter, UINT64 flowContext,
							  FWPS_CLASSIFY_OUT0* classifyOut );
NTSTATUS NetNotifyBindCallback( FWPS_CALLOUT_NOTIFY_TYPE notifyType,const GUID* filterKey,FWPS_FILTER1* filter );
VOID NetFlowDeleteBindCallback( IN UINT16 layerId, IN UINT32 calloutId, IN UINT64 flowContext );
//文件
FLT_PREOP_CALLBACK_STATUS FilePreCallback( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext );
FLT_POSTOP_CALLBACK_STATUS FilePostCallback( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID CompletionContext, FLT_POST_OPERATION_FLAGS Flags );
NTSTATUS FileInstanceSetup( PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_SETUP_FLAGS Flags, DEVICE_TYPE VolumeDeviceType, FLT_FILESYSTEM_TYPE VolumeFilesystemType );
NTSTATUS FileInstanceQueryTeardown( PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags );
VOID FileInstanceTeardownStart( PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags );
VOID NPInstanceTeardownComplete( PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags );
NTSTATUS FileUnloadCallback( FLT_FILTER_UNLOAD_FLAGS Flags );

EXTERN_C_END