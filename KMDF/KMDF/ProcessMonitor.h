#pragma once
#include "ObjectInterface.h"

class CProcessMonitor:public CObjectInterface
{
public:
	CProcessMonitor(CListContainer *pList);
	virtual ~CProcessMonitor();

public:
	virtual bool init( );
	virtual void Release( );

	virtual void ProcessMonitorData(const PVOID handle, const PVOID pCreateinfo);

protected:
	bool CProcessMonitor::GetProcessImagePath(HANDLE dwProcessId, PUNICODE_STRING &ProcessImagePath);
private:
	ZwQueryInformationProcess m_pZwQueryInformationProcessFn;
};
