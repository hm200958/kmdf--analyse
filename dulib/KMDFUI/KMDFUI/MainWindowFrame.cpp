#include "StdAfx.h"
#include "MainWindowFrame.h"
#include "ProcessEnum.h"

CMainWindowFrame::CMainWindowFrame()
{
	m_pListProc = nullptr;
	m_pListServ = nullptr;
	m_pListProcAnaly = nullptr;
	m_pListPcap = nullptr;
	m_pListNetLink = nullptr;

}
CMainWindowFrame::~CMainWindowFrame()
{
	ReleasePtr();
}
void CMainWindowFrame::InitWindow()
{
	m_pListProc = static_cast<CListUI *>(m_PaintManager.FindControl(_T("domainlist_proc")));
	m_pListServ = static_cast<CListUI *>(m_PaintManager.FindControl(_T("domainlist_service")));
	m_pListProcAnaly = static_cast<CListUI *>(m_PaintManager.FindControl(_T("domainlist_proc_analyse")));
	m_pProcessTree = static_cast< CTreeViewUI * >( m_PaintManager.FindControl( _T( "ProcessTree" ) ) );
	m_pListPcap = static_cast<CListUI *>(m_PaintManager.FindControl(_T("domainlist_procpcap")));
	m_pListNetLink = static_cast<CListUI *>(m_PaintManager.FindControl(_T("domainlist_netlink")));

	
	InitPresenter();
}
void CMainWindowFrame::InitPresenter()
{
	m_hThreadObj.CreateThread( );
}
void CMainWindowFrame::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}
void CMainWindowFrame::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click")) 
	{
		if (msg.pSender->GetName() == _T("closebtn"))
		{
			m_hThreadObj.SetStopFlag( TRUE );
			m_hThreadObj.CloseThread( );
			PostQuitMessage(0);
			return;
		}
		else if (msg.pSender->GetName() == _T("minbtn")) {
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
		}
		else if (msg.pSender->GetName() == _T("maxbtn")) {
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return;
		}
		else if (msg.pSender->GetName() == _T("restorebtn")) {
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return;
		}
	
	}
	else if (msg.sType == _T("selectchanged"))
	{
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("examine"))
			pControl->SelectItem(0);
		else if (name == _T("trojan"))
			pControl->SelectItem(1);
		else if (name == _T("plugins"))
			pControl->SelectItem(2);
		else if (name == _T("vulnerability"))
			pControl->SelectItem(3);
		else if (name == _T("rubbish"))
			pControl->SelectItem(4);
	}
	else
	{
		__super::Notify( msg );
	}
	
}
LRESULT CMainWindowFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	return S_OK;
}
CDuiString CMainWindowFrame::GetSkinFolder()
{
	return CPaintManagerUI::GetInstancePath() + _T("skin");
}
CDuiString CMainWindowFrame::GetSkinFile()
{
	return _T("littelbirds.xml");
}
LPCTSTR CMainWindowFrame::GetWindowClassName(void) const
{
	return _T("littlebirdsflying");
}

void CMainWindowFrame::ReleasePtr()
{
	m_pListProc = nullptr;
	m_pListServ = nullptr;
	m_pListProcAnaly = nullptr;
	m_pListPcap = nullptr;
	m_pListNetLink = nullptr;
	m_pProcessTree = nullptr;
}