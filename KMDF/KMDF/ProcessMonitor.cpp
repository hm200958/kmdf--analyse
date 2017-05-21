#include <Ntifs.h>
#include "ProcessMonitor.h"
#include "DriverDataDefine.h"
#include "MonitorCallBack.h"


CProcessMonitor::CProcessMonitor(CListContainer *pList):CObjectInterface(pList)
{
	m_pZwQueryInformationProcessFn = nullptr;
}

CProcessMonitor::~CProcessMonitor()
{
	//Release( );
}
bool CProcessMonitor::init()
{
	KdPrint(("ProcessMonitor::init\n"));
	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, FALSE);
	if (!NT_SUCCESS( status ))
	{
		if ( status == STATUS_INVALID_PARAMETER )
		{
			KdPrint(("PsSetCreateProcessNotifyRoutineEx fialed STATUS_INVALID_PARAMETER \n", status));
		}
		else if (status == STATUS_ACCESS_DENIED )
		{
			KdPrint(("PsSetCreateProcessNotifyRoutineEx fialed STATUS_ACCESS_DENIED \n", status));
		}
		else
		{
			KdPrint(("PsSetCreateProcessNotifyRoutineEx fialed %0x \n", status));
		}
		return false;
	}
	if (nullptr == m_pZwQueryInformationProcessFn)
	{
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"ZwQueryInformationProcess");
		m_pZwQueryInformationProcessFn=(ZwQueryInformationProcess)MmGetSystemRoutineAddress(&routineName);

		if (nullptr == m_pZwQueryInformationProcessFn)
		{
			DbgPrint("Cannot resolve ZwQueryInformationProcess\n");
			return false;
		}
	}

	return true;
}
void CProcessMonitor::Release()
{
	KdPrint(("ProcessMonitor::Release\n"));
	PsSetCreateProcessNotifyRoutineEx(CreateProcessNotifyEx, TRUE);
	m_pZwQueryInformationProcessFn = nullptr;
	__super::Release( );
}
void CProcessMonitor::ProcessMonitorData(const PVOID handle, const PVOID pCreateinfo)
{
	if ( m_pListContainer != nullptr )
	{
		if (pCreateinfo == nullptr)
		{
			PPROCESS_DATA pdata = PPROCESS_DATA(GetDataBuffer(sizeof(ProcessData)));
			if (pdata != nullptr)
			{
				pdata->unit.eType = DataType::DATA_TYPE_PROCESS_Exit;
				pdata->unit.nLength = sizeof(ProcessData);
				pdata->uProceId = (UINT32)handle;

				InsertDataList( (BYTE*)pdata );
			}
			/*½ø³ÌÍË³ö*/
			KdPrint(("CProcessMonitor::ProcessMonitorData [Destroy][ProcessId = %d]\n",(HANDLE)handle));
		}
		else
		{
			PPS_CREATE_NOTIFY_INFO pCreate = PPS_CREATE_NOTIFY_INFO(pCreateinfo);
			ULONG lTotal = sizeof(ProcessData);
			ULONG lCmd = 0;
			ULONG lprocFile = 0;
			ULONG lParentFile = 0;

			if (pCreate->CommandLine != nullptr )
			{
				lCmd = pCreate->CommandLine->Length + 2;
				lTotal += lCmd;
			}
			if ( pCreate->ImageFileName != nullptr )
			{
				lprocFile = pCreate->ImageFileName->Length + 2; 
				lTotal += lprocFile;
			}
			else
			{
				//pCreate->FileObject 
			}
			PUNICODE_STRING pParentStr = nullptr;
			if (GetProcessImagePath( pCreate->CreatingThreadId.UniqueProcess,pParentStr ) && pParentStr != nullptr )
			{
				lParentFile = pParentStr->Length + 2;
				lTotal += lParentFile;
			}

			PPROCESS_DATA pData = PPROCESS_DATA( GetDataBuffer(lTotal));
			if ( pData != nullptr )
			{
				pData->unit.eType = DataType::DATA_TYPE_PROCESS_Create;
				pData->unit.nLength = lTotal;

				pData->uParentId = (UINT32)(pCreate->CreatingThreadId.UniqueProcess);
				pData->uProceId = (UINT32)handle;

				if (lCmd > 0)
				{
					pData->uCMDLineOffset = sizeof(ProcessData);
					RtlCopyMemory(((BYTE *)pData + pData->uCMDLineOffset), pCreate->CommandLine->Buffer, lCmd-2);
				}
				if (lprocFile > 0)
				{
					pData->uProcessFileOffset = sizeof( ProcessData ) + lCmd;
					RtlCopyMemory(((BYTE *)pData + pData->uProcessFileOffset), pCreate->ImageFileName->Buffer, lprocFile-2);
				}
				if (lParentFile > 0)
				{
					pData->uParentFileOffset = sizeof( ProcessData ) + lCmd + lprocFile ;
					RtlCopyMemory(((BYTE *)pData + pData->uParentFileOffset), pParentStr->Buffer, lParentFile-2);
					ExFreePoolWithTag(pParentStr,ALLOCTAG);
				}

				InsertDataList( (BYTE*)pData );
				KdPrint(("CProcessMonitor::ProcessMonitorData [Create][ParentID %d][ProcessId = %d,ProcessName=%wZ][Cmdlines=%wZ]\n",
					pData->uParentId, handle, pCreate->ImageFileName, pCreate->CommandLine));
			}
			
		}
	}
}
bool CProcessMonitor::GetProcessImagePath(HANDLE dwProcessId, PUNICODE_STRING &ProcessImagePath)
{
	bool bRet = false;

	if (m_pZwQueryInformationProcessFn != nullptr)
	{
		HANDLE hProcess = nullptr;
		PEPROCESS pEprocess = nullptr;
		ULONG returnedLength = 0;
		BYTE *pData = nullptr;

		NTSTATUS Status = PsLookupProcessByProcessId(dwProcessId, &pEprocess);
		if (NT_SUCCESS(Status))
		{
			Status = ObOpenObjectByPointer(pEprocess, OBJ_KERNEL_HANDLE, NULL, GENERIC_READ, *PsProcessType, KernelMode, &hProcess);
			if (NT_SUCCESS(Status))
			{
				Status = m_pZwQueryInformationProcessFn(hProcess, ProcessImageFileName, NULL, 0, &returnedLength);

				if (STATUS_INFO_LENGTH_MISMATCH == Status)
				{

					pData = (BYTE *)ExAllocatePoolWithTag(PagedPool, returnedLength, ALLOCTAG);

					if (nullptr != pData)
					{
						Status = m_pZwQueryInformationProcessFn(hProcess, ProcessImageFileName, pData, returnedLength, &returnedLength);
						if (NT_SUCCESS(Status))
						{
							ProcessImagePath = (PUNICODE_STRING)pData;
							bRet = true;
						}
					}
				}

				ZwClose(hProcess);
			}
		}
		
	}
	return bRet;
}