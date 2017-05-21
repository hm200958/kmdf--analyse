#pragma once

#include "InterfacePresenter.h"

class CProcessEnum :public IInterfacePresenter
{
public:
	CProcessEnum();
	virtual ~CProcessEnum();

public:
	virtual BOOL GetProcessEnumData();
	virtual void ReleaseData( );

};