#pragma once
#include "ObjectInterface.h"

class CRegisterMonitor :public CObjectInterface
{
public:
	CRegisterMonitor( CListContainer *pList );
	virtual ~CRegisterMonitor( );
public:
	virtual bool init( );
	virtual void Release( );

	virtual void RegisterMonitorData( REG_NOTIFY_CLASS regType, const PVOID pData );
protected:
	bool GetObjectPathName( PVOID pObject, PUNICODE_STRING &pReturn );
	bool FormatDataToRegisterData( DataType eType, PUNICODE_STRING pPath = nullptr, PUNICODE_STRING pValueName = nullptr, PVOID pData= nullptr, ULONG uDataLength= 0 ,ULONG eDataType = REG_NONE );
	
	void FilterDeleteKey( const PVOID pData );
	void FilterSetValueKey( const PVOID pData );
	void FilterDeleteValueKey( const PVOID pData );
	void FilterQueryKey( const PVOID pData );
	void FilterQueryValueKey( const PVOID pData );
	void FilterCreateKey( const PVOID pData );
	void FilterOpenKey( const PVOID pData );
	void FilterCreateKeyEx( const PVOID pData );
	void FilterOpenKeyEx( const PVOID pData );
protected:
	LARGE_INTEGER m_CallCooke;
};