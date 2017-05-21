#pragma once
#include "interfaceModel.h"
#include "UserStructDefine.h"
#include "NetWFPFilter.h"

class CModelData:public IIinterfaceModel
{
public:
	CModelData( );
	virtual ~CModelData( );

public:
	virtual BOOL DeviceControl( DWORD IoControlCode, PVOID pData, DataType eType = DataType::DATA_TYPE_MIN );
	virtual BOOL ReadData( );
	virtual BOOL WriteData( PVOID pData );

	virtual void CloseDevice( );
	virtual BOOL CreateDevice( );

protected:
	virtual BOOL DeviceControl( DWORD dwIoControlCode, LPVOID lpBuffer, DWORD BufferSize, LPDWORD lpBytesReturned, BOOL bIn );
	virtual BOOL ReadData(BYTE *pByteBuffer, DWORD dwBufferLength, DWORD *pReturnLength );
	virtual BOOL WriteData(BYTE *pByteBuffer, DWORD dwBufferLength );

protected:
	void DispatchPresenterByData( BYTE *pData );
private:
	HANDLE m_hDevice;
	CNetWFPFilter m_objNetFilter;
};