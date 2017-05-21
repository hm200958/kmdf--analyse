#pragma once
#include "interfaceModel.h"
#include "UserStructDefine.h"
#include "InterfaceView.h"

class GlobalMap
{
public:
	GlobalMap();
	~GlobalMap();

public: 
	void CreateInterface( IInterfaceView *pView );
	IInterfacePresenter *GetPagePresenter( PAGE_TYPE etype );
	IIinterfaceModel * GetModelInterface( );

	void ReleaseModel();
	void ClearPage();
protected:
	void AddPage( PAGE_TYPE etype, IInterfacePresenter *pPresenter );
	
private:
	MAP_PagePresenter m_mapPage;
	IIinterfaceModel *m_pInterModel= nullptr;
};

extern GlobalMap g_GlobalMap;
