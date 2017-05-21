// littlebirds.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "littlebirds.h"
#include "MainWindowView.h"
#define MAX_LOADSTRING 100

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CPaintManagerUI::SetInstance(hInstance);

	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	CMainWindowFrame* pFrame = new CMainWindowView();
	if (pFrame == NULL)
		return 0;
	pFrame->Create(NULL, _T("littlebirds"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 1024, 738);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOWNORMAL);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}

