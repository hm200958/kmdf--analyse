#pragma once
#include "ObjectInterface.h"
class CFileMonitor :public CObjectInterface
{
public:
	CFileMonitor( CListContainer *pList );
	virtual ~CFileMonitor( );

public:
	virtual bool init( );
	virtual void Release( );
	virtual void FileMonitorData( PFLT_CALLBACK_DATA  pData, PCFLT_RELATED_OBJECTS FltObjects, PVOID *CompletionContext, bool bPre );
protected:

	void PreFileMonitorData( PFLT_CALLBACK_DATA  pData );
	void PostFileMonitorData( PFLT_CALLBACK_DATA  pData ); //查询文件信息时要用到post操作 因为在post时已经完成文件的信息查询 此时的paramter里有查询的数据

	bool GetDataTypeByIoParameter(const PFLT_IO_PARAMETER_BLOCK  pData, DataType &eType, FileDataType &eFileType, UINT32 &uFileAttribute,BYTE *&pRetrnData,UINT32 &uReturnLegth );
	void AnalyseFileData( PFLT_CALLBACK_DATA  pData );
	void FormatFileMonitorData( UNICODE_STRING &pFileName,DataType eType, FileDataType eFileType, UINT32 uFileAttribute,BYTE *pData = nullptr, UINT32 uDataLength  = 0);
private:
	PFLT_FILTER m_pFileRegFilter;
};