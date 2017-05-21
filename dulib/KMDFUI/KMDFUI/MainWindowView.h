#pragma once
#include "InterfaceView.h"
#include "MainWindowFrame.h"

class CMainWindowView :public CMainWindowFrame, public IInterfaceView
{
public:
	CMainWindowView( );
	virtual ~CMainWindowView( );

public:
	virtual void Notify( TNotifyUI& msg );
	virtual void InsertProcessTree( ProcessCreate *pData );
	virtual void InsertListProcessData( ProcessCreate *pData );
	virtual void InsertListRegisterData( RegisterMonitor *pData );
	virtual void InsertListNetData( NetMonitor *pData );
	virtual void InsertListFileData( FileMonitor *pData );

	virtual bool CheckCurProcessSel( UINT32 uPid );
protected:
	virtual void InitPresenter( );
	virtual void ReleasePtr( );

protected:
	CTreeNodeUI *FindProcessItemById( CTreeNodeUI *pRoot, UINT32 uParentId );
	void InsertListParentProcess( ProcessCreate *pData, BOOL bParent = TRUE );
	void LoadCurSelProcessData( UINT32 iProcessId );
	void InsertListData( CListUI *pList, LPCTSTR str1, LPCTSTR str2, LPCTSTR str3 );
private:
	CTreeNodeUI *m_pProcessTreeRoot;
};