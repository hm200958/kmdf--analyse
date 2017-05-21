#pragma once
#include "DriverOperator.h"
#include "ObjectInterface.h"
#include <wdftypes.h>
#include "ListContainer.h"


class CKmdfManager:public CDriverMem
{
public:
	CKmdfManager();
	virtual ~CKmdfManager();

public:
	BYTE * GetFilterData();
	VOID InitManager();
	NTSTATUS ProcessControlIrp(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
	NTSTATUS ProcessReadDataBuffer(IN PIRP Irp, PIO_STACK_LOCATION IrpStack,bool bContrl = FALSE);
	NTSTATUS ProcessReadDataDirect( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl = FALSE );
	NTSTATUS ProcessReadDataNeither( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl = FALSE );
	NTSTATUS ProcessWriteDataBuffer(IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl = FALSE );
	NTSTATUS ProcessWriteDataDirect( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl = FALSE );
	NTSTATUS ProcessWriteDataNeither( IN PIRP Irp, PIO_STACK_LOCATION IrpStack, bool bContrl = FALSE );
public:
	void ProcessMonitor(__in HANDLE  ProcessId, const PPS_CREATE_NOTIFY_INFO pCreateInfo );
	void RegisterMonitor( REG_NOTIFY_CLASS regType, const PVOID pData );
	void NetMoniter( const FWPS_INCOMING_VALUES  *inFixedValues, const FWPS_INCOMING_METADATA_VALUES  *inMetaValues,PVOID pNetrData,OUT FWPS_CLASSIFY_OUT  *classifyOut, DataType eType );
	void FileMonitor( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, bool bPre = true );
protected:
	void ReleaseObject( );
	void CopyDataToUser(PVOID const Buffer, PUNIT_FLAG const pData);
private:
	CListContainer *m_pListContainer;
	CObjectInterface *m_pProcessMoniter;
	CObjectInterface *m_pRegisterMoniter;
	CObjectInterface *m_pNetmoniter;
	CObjectInterface *m_pFileMoniter;
};
extern CKmdfManager *g_CKMDFmanager;
