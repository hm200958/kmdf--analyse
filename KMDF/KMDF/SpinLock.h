#pragma once
#include <wdm.h>
#include "DriverOperator.h"
class  CSpinLock:public CDriverMem
{
public:
	CSpinLock( KSPIN_LOCK *Spin )
	{
		if ( Spin != nullptr )
		{
			m_pSpin = Spin;
			KeAcquireSpinLock(m_pSpin, &m_oldIrql);
		}
		
	}
	virtual ~CSpinLock( )
	{
		if ( m_pSpin != nullptr )
		{
			KeReleaseSpinLock(m_pSpin, m_oldIrql);
			m_pSpin = nullptr;
		}
	}

private:
	PKSPIN_LOCK m_pSpin;
	KIRQL m_oldIrql;
};
