#pragma once
#include "DriverOperator.h"
class CProtocolAnalysis:public CDriverMem
{
public:
	CProtocolAnalysis( );
	virtual ~CProtocolAnalysis( );

	bool GetDnsNameData( BYTE *pData, BYTE *&pRetuned, UINT32 &duReturnLen );

protected:
	void ntoh( BYTE *pNet, BYTE *pHost, UINT16 nLength );
private:

};

