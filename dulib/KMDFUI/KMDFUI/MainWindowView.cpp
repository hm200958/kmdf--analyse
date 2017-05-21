#include "StdAfx.h"
#include "MainWindowView.h"
#include "ProcessEnum.h"
#include "ProcessBehaviour.h"
#include "ModelData.h"
#include "GlobalMap.h"
#include "GlobalFun.h"

CMainWindowView::CMainWindowView()
{

}
CMainWindowView::~CMainWindowView()
{

}
void CMainWindowView::Notify( TNotifyUI& msg )
{
	bool bTrue = false;
	//MessageBox( NULL, msg.pSender->GetName().GetData( ), NULL, MB_OK );
 	if ( msg.sType == DUI_MSGTYPE_ITEMSELECT )
	{
		if ( msg.pSender == m_pProcessTree )
		{
			int ninde = m_pProcessTree->GetCurSel( );
			CTreeNodeUI *pNode = ( CTreeNodeUI * )m_pProcessTree->GetItemAt( ninde );
			if ( pNode != m_pProcessTreeRoot )
			{
				LoadCurSelProcessData( UINT32( pNode->GetTag( )));
			}

			bTrue = true;
		}
 	}

	if ( !bTrue )
	{
		__super::Notify( msg );
	}
}
void CMainWindowView::InsertProcessTree( ProcessCreate *pData )
{
	if ( pData != nullptr && m_pProcessTreeRoot != nullptr )
	{
		if ( pData->eType == DATA_TYPE_PROCESS_Create )
		{
			CTreeNodeUI *pnode = new CTreeNodeUI;
			if ( pnode != nullptr )
			{
				CDuiString strName = PathFindFileName( pData->strProcessFile.GetString( ) );
				pnode->SetItemText( strName );
				pnode->SetAttribute( _T( "folderattr" ), TreeFlag );
				pnode->SetVisibleTag( true );
				pnode->SetTag( UINT_PTR( pData->uProceId ) );

				CTreeNodeUI *pFind = FindProcessItemById( m_pProcessTreeRoot, pData->uParentId );
				if ( pFind )
				{
					pFind->AddAt( pnode, pFind->GetCountChild( ) );
					if ( pFind == m_pProcessTree->GetItemAt( m_pProcessTree->GetCurSel( ) ) )
					{
						InsertListParentProcess( pData );
					}
				}
				else
				{
					m_pProcessTreeRoot->AddAt( pnode, m_pProcessTreeRoot->GetCountChild( ) );
				}
			}
		}
		else
		{
			CTreeNodeUI *pFind = FindProcessItemById( m_pProcessTreeRoot, pData->uProceId );
			if ( pFind )
			{
				if ( pFind == m_pProcessTree->GetItemAt( m_pProcessTree->GetCurSel( ) ) )
				{
					InsertListParentProcess( pData,FALSE );
				}
			}
		}
		
	}
}
void CMainWindowView::InsertListProcessData( ProcessCreate *pData )
{
	InsertListParentProcess( pData,FALSE );
}
void CMainWindowView::InsertListRegisterData( RegisterMonitor *pData )
{
	if ( m_pListProcAnaly != nullptr && pData != nullptr )
	{
		CString strDec = pData->strPath;
		strDec += _T( ":" );
		strDec += pData->strName;
		strDec += GetRegString( pData->eRegType, pData->pbuffdata ).c_str( );
		InsertListData( m_pListProcAnaly, std::to_wstring( m_pListProcAnaly->GetCount( ) ).c_str( ), GetStringType( pData->eType ).c_str( ), strDec.GetString( ) );
	}
}
void CMainWindowView::InsertListNetData( NetMonitor *pData )
{
	if ( m_pListProcAnaly != nullptr && pData != nullptr )
	{
		InsertListData( m_pListProcAnaly, std::to_wstring( m_pListProcAnaly->GetCount( ) ).c_str( ), GetStringType( pData->eType ).c_str( ), GetNetString( pData ).c_str( ) );
	}
}
void CMainWindowView::InsertListFileData( FileMonitor *pData )
{
	if ( m_pListProcAnaly != nullptr && pData != nullptr )
	{
		InsertListData( m_pListProcAnaly, std::to_wstring( m_pListProcAnaly->GetCount( ) ).c_str( ), GetStringType( pData->eType ).c_str( ), GetFileString( pData ).c_str( ) );
	}
}
bool CMainWindowView::CheckCurProcessSel( UINT32 uPid )
{
	if ( uPid > 0 )
	{
		int ninde = m_pProcessTree->GetCurSel( );
		CTreeNodeUI *pNode = ( CTreeNodeUI * )m_pProcessTree->GetItemAt( ninde );
		if ( pNode != nullptr && UINT32( pNode->GetTag( ) ) == uPid )
		{
			return true;
		}
	}
	return false;
}
void CMainWindowView::InitPresenter( )
{
	g_GlobalMap.CreateInterface( this );
	__super::InitPresenter( );

	m_pProcessTreeRoot = new CTreeNodeUI;
	if ( m_pProcessTreeRoot != nullptr )
	{
		m_pProcessTreeRoot->SetAttribute( _T( "folderattr" ), TreeFlag );
		m_pProcessTreeRoot->SetItemText( _T( "½ø³ÌÊ÷" ) );
		m_pProcessTreeRoot->SetTreeView( m_pProcessTree );
		m_pProcessTreeRoot->SetTag( UINT_PTR( -1 ) );
		m_pProcessTree->Add( m_pProcessTreeRoot );
	}

	m_pProcessTree->SetVisible(true);
	m_pProcessTreeRoot->Expand( );
}
CTreeNodeUI *CMainWindowView::FindProcessItemById(CTreeNodeUI *pRoot, UINT32 uParentId )
{
	CTreeNodeUI *pNode = nullptr;
	if ( pRoot != nullptr && uParentId > 0 )
	{
		for ( int i = 0; i< pRoot->GetCountChild( ); ++i )
		{
			pNode = pRoot->GetChildNode( i );
			if ( pNode->GetTag() == UINT_PTR( uParentId ))
			{
				break;
			}

			pNode = FindProcessItemById( pNode, uParentId );
			if ( pNode != nullptr )
			{
				break;
			}
		}
	}

	return pNode;
}
void CMainWindowView::InsertListParentProcess( ProcessCreate *pData,BOOL bParent )
{
	if ( m_pListProcAnaly != nullptr && pData != nullptr )
	{
		if ( bParent )
		{
			InsertListData( m_pListProcAnaly, std::to_wstring( m_pListProcAnaly->GetCount( ) ).c_str( ), GetStringType( pData->eType ).c_str( ), pData->strProcessFile.GetString( ) );
		}
		else
		{
			InsertListData( m_pListProcAnaly, std::to_wstring( m_pListProcAnaly->GetCount( ) ).c_str( ), GetStringType( pData->eType ).c_str( ), pData->strCmd.GetString( ) );
		}
		
	}
}
void CMainWindowView::LoadCurSelProcessData( UINT32 iProcessId )
{
	if ( m_pListProcAnaly != nullptr )
	{
		m_pListProcAnaly->RemoveAll( );

		IInterfacePresenter * pProcess = g_GlobalMap.GetPagePresenter( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour );
		if ( pProcess != nullptr )
		{
			pProcess->LoadProcessData( iProcessId );
		}
	}
}
void CMainWindowView::InsertListData( CListUI *pList, LPCTSTR str1, LPCTSTR str2, LPCTSTR str3 )
{
	if ( ( pList != nullptr ) && ( str1 != nullptr ) && ( str2 != nullptr ) && ( str3 != nullptr ) )
	{
			CListTextElementUI *pItem = new CListTextElementUI;
			if ( pItem != nullptr )
			{
				pItem->SetOwner( pList );

				pItem->SetText( 0, str1 );
				pItem->SetText( 1, str2 );
				pItem->SetText( 2, str3 );
				
				pList->Add( pItem );
				pItem->SetVisible( true );
			}
	}
}
void CMainWindowView::ReleasePtr( )
{
	g_GlobalMap.ClearPage( );
	g_GlobalMap.ReleaseModel( );
	__super::ReleasePtr( );
}