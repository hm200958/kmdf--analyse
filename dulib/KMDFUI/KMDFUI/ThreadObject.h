#pragma once
class CThreadObject
{
public:
	CThreadObject( );
	virtual ~CThreadObject( );
public:
	static void ModelWorkThread( void *pData );
	VOID CreateThread( );
	VOID CloseThread( );
	VOID SetStopFlag( BOOL bStop );
	BOOL GetStopFlag( );
private:
	HANDLE m_hThread;
	BOOL m_bStop;
};
