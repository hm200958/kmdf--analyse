#pragma once
#include "UserStructDefine.h"
//#include <kernelspecs.h>
#include "fwpmu.h"
class CNetWFPFilter
{
public:
	CNetWFPFilter(  );
	virtual ~CNetWFPFilter( );

	bool init( );
	void Release( );
protected:

	void InitConnectWfp( );
	void InitBindWfp( );

	void ReleaseWfp( );

	bool OpenWfpEngine( );
	bool AddCallOutToEngine( PTCHAR pwCharName, PTCHAR pwCharDes, const GUID &CallGuid, const GUID &LayerGuid, UINT32 *pAddCallOutID );
	bool AddSubLayerToEngine( PTCHAR pwCharName, PTCHAR pwCharDes, const GUID &subLayerGuid, UINT16 uWeight );
	bool AddFitrerToEngine( PTCHAR pwCharName, PTCHAR pwCharDes, const GUID &callGuid, const GUID &bindLayerGuid, const GUID &subLayerGuid, UINT32 uNumFilter, FWPM_FILTER_CONDITION *filterCondition, UINT64 *pAddFilterID );
private:
	NetMonitorCallbackIDArry m_stCallbackID[ NetMonitorCallbackIDNum ];
	HANDLE m_hEngine;
};
