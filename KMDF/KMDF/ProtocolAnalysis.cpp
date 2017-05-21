#include "ProtocolAnalysis.h"
#include "ProtocolHeader.h"
CProtocolAnalysis::CProtocolAnalysis( )
{

}
CProtocolAnalysis::~CProtocolAnalysis( )
{

}
bool CProtocolAnalysis::GetDnsNameData( BYTE *pData, BYTE *&pRetuned, UINT32 &duReturnLen )
{
	duReturnLen = 0;
	if ( pData != nullptr )
	{
		UINT16 uNum = 0;
		PDNS_HEADER pDns = PDNS_HEADER( pData + sizeof( UDP_HEADER ) );
		ntoh( ( BYTE * )&( pDns->usQuNum ), (BYTE *)&uNum, 2 );
		
		if ( uNum > 0 )
		{
			pRetuned = pData + sizeof( UDP_HEADER ) + sizeof( DNS_HEADER ) + 1;

			while ( *( pRetuned + duReturnLen ) > 0 )
			{
				++duReturnLen;
			}
			++duReturnLen;

			return true;
		}
	}

	return false;
}
void CProtocolAnalysis::ntoh( BYTE *pNet, BYTE *pHost, UINT16 nLength )
{
	if (( pNet != nullptr )&&( pHost != nullptr )&&( nLength > 0 ))
	{
		for ( int i = 0; i < nLength ; ++i )
		{
			*( pHost + ( nLength - 1 - i ) ) = *( pNet + i );
		}
	}
}