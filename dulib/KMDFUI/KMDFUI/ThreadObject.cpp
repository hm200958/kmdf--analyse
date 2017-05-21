#include "StdAfx.h"
#include "ThreadObject.h"
#include "GlobalMap.h"

CThreadObject::CThreadObject( )
{
	m_hThread = nullptr;
	m_bStop = FALSE;
}
CThreadObject::~CThreadObject( )
{
	CloseThread( );
}
void CThreadObject::ModelWorkThread( void* pData )
{
	CThreadObject *pThread = ( CThreadObject * )pData;
	if ( pThread != nullptr )
	{
		IIinterfaceModel *pModel = g_GlobalMap.GetModelInterface( );
		if (( pModel != nullptr ) && ( pModel->CreateDevice( )))
		{
			while ( !pThread->GetStopFlag( ))
			{
				if ( !pModel->ReadData( ) && !pThread->GetStopFlag( ))
				{
					Sleep( 1000 );
				}
			}

			pModel->CloseDevice( );
		}
		CString  str;
		str.Format( _T( "ui Create Device failed %d" ), GetLastError( ) );
		OutputDebugString( str );
	}
}
VOID CThreadObject::CreateThread( )
{
	m_hThread =(HANDLE)_beginthread( &CThreadObject::ModelWorkThread, 0, this );
}
VOID CThreadObject::SetStopFlag( BOOL bStop )
{
	m_bStop = bStop;
}
BOOL CThreadObject::GetStopFlag( )
{
	return m_bStop;
}
VOID CThreadObject::CloseThread( )
{
	if ( HANDLE_SUCCESS( m_hThread ))
	{
		WaitForSingleObject( m_hThread, INFINITE );
		//CloseHandle( m_hThread );
		m_hThread = nullptr;
	}
}
