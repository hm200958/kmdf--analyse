#include "StdAfx.h"
#include "GlobalMap.h"
#include "ProcessEnum.h"
#include "ProcessBehaviour.h"
#include "ModelData.h"

GlobalMap g_GlobalMap;

GlobalMap::GlobalMap( )
{
	m_pInterModel = nullptr;
	m_mapPage.clear( );
}
GlobalMap::~GlobalMap( )
{
	ReleaseModel( );
	ClearPage( );
}
void GlobalMap::CreateInterface( IInterfaceView *pView )
{
	if ( pView != nullptr )
	{
		m_pInterModel = new CModelData();
		IInterfacePresenter *pInterPresenter = new CProcessEnum;
		if (pInterPresenter != nullptr)
		{
			pInterPresenter->InitView( pView );
			pInterPresenter->InitModel( m_pInterModel );

			g_GlobalMap.AddPage( PAGE_TYPE::PAGE_TYPE_ProcessEnum, pInterPresenter );
		}

		pInterPresenter = new CProcessBehaviour( );
		if (pInterPresenter != nullptr)
		{
			pInterPresenter->InitView( pView );
			pInterPresenter->InitModel( m_pInterModel );

			g_GlobalMap.AddPage( PAGE_TYPE::PAGE_TYPE_ProcessBehaviour, pInterPresenter );
		}
	}
	
}
void GlobalMap::AddPage(PAGE_TYPE etype, IInterfacePresenter *pPresenter)
{
	if (( etype != PAGE_TYPE::PAGE_TYPE_Min ) && ( pPresenter != nullptr ))
	{
		auto it = m_mapPage.find( etype );
		if ( it == m_mapPage.end( ))
		{
			m_mapPage.insert( std::make_pair( etype, pPresenter ));
		}
	}
}
IInterfacePresenter *GlobalMap::GetPagePresenter( PAGE_TYPE etype )
{
	if ( etype != PAGE_TYPE::PAGE_TYPE_Min )
	{
		auto it = m_mapPage.find( etype );
		if ( it != m_mapPage.end( ))
		{
			return it->second;
		}
	}
	return nullptr;
}
IIinterfaceModel *GlobalMap::GetModelInterface( )
{
	return m_pInterModel;
}
void GlobalMap::ClearPage()
{
	auto it = m_mapPage.begin( );
	while (it != m_mapPage.end( ))
	{
		it->second->ReleaseData( );
		delete it->second;
		++it;
	}
	m_mapPage.clear();
}
void GlobalMap::ReleaseModel( )
{
	if ( m_pInterModel != nullptr )
	{
		delete m_pInterModel;
		m_pInterModel = nullptr;
	}
}