#pragma once
#include "UIlib.h"
#include "ThreadObject.h"

using namespace DuiLib;

class CMainWindowFrame :public WindowImplBase
{
public:
	CMainWindowFrame();
	virtual ~CMainWindowFrame();

	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitPresenter();

protected:
	virtual void ReleasePtr();

protected:
	CListUI *m_pListProc;
	CListUI *m_pListServ;
	CListUI *m_pListProcAnaly;
	CTreeViewUI *m_pProcessTree;
	CListUI *m_pListPcap;
	CListUI *m_pListNetLink;


	CThreadObject m_hThreadObj;
};