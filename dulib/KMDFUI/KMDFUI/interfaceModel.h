#pragma once
/*****
该模块是数据获取模块 用于和下层的驱动交互 获取驱动采集的数据
******/
#include "UserDataDefine.h"
interface IIinterfaceModel
{
public:
	virtual BOOL DeviceControl( DWORD IoControlCode, PVOID pData, DataType eType = DataType::DATA_TYPE_MIN ) = 0;
	virtual BOOL ReadData( ) = 0;
	virtual BOOL WriteData( PVOID pData ) = 0;
	virtual void CloseDevice( ) = 0;
	virtual BOOL CreateDevice( ) = 0;
};