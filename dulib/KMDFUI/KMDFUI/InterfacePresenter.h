#pragma once
#include "InterfaceView.h"
#include "interfaceModel.h"
/*
该模块为逻辑处理模块 用于处理从model取得数据 经过加工处理后 把数据推送给view做展示
*/
interface IInterfacePresenter
{
public:
	IInterfacePresenter() 
	{
		m_pInterView = nullptr;
		m_pInterModel = nullptr;
	};
	virtual ~IInterfacePresenter()
	{
		ReleaseData( );
		m_pInterView = nullptr;
		m_pInterModel = nullptr;
	};

public://model 
	virtual void InitView( IInterfaceView *pView ) { m_pInterView = pView; };
	virtual void InitModel( IIinterfaceModel *pModel ) { m_pInterModel = pModel; };
	virtual void ReleaseData( ) {};
	
	//进程
	virtual void LoadProcessData( UINT32 uProcessId ) {};//view
	virtual void ProcessCreateData( ProcessCreate *pData ) {};//model
	//注册表
	virtual void ProcessRegisterData( RegisterMonitor *pData ) {}
	//网络
	virtual void ProcessNetData( NetMonitor *pData ) {};
	//文件
	virtual void ProcessFileData( FileMonitor *pData ) { };
protected:
	IInterfaceView *m_pInterView = nullptr;
	IIinterfaceModel *m_pInterModel = nullptr;
};