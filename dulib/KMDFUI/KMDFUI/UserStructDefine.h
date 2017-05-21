#pragma once
#include <map>
#include "UserDataDefine.h"
#include "InterfacePresenter.h"

#define TreeFlag _T("padding=\"0,1,0,0\" width=\"16\" height=\"16\" normalimage=\"file='treeview_b.png' source='0,0,16,16'\" hotimage=\"file='treeview_b.png' source='16,0,32,16'\" selectedimage=\"file='treeview_a.png' source='0,0,16,16' selectedhotimage=\"file='treeview_a.png' source='16,0,32,16'\"")

#define  DRIVER_NAME L"\\Device\\KMDFService"
#define  DRIVER_SYMBOLICLINK L"\\??\\KMDFDOSService"

#define  NETCONNECTNAME L"Connect Call Out"
#define  NETCONNECTDES L"Connect Notify Process"

#define  NETCONNECTSUBLAYERNAME L"Connect Sublayer"
#define  NETCONNECTSUBLAYERDES L"Connect Sublayer Add"

#define  NETCONNECTFILTERNAME L"Connect Filter"
#define  NETCONNECTFILTERDES L"Connect Filter Bind"

#define  NETBINDNAME L"Bind Call Out"
#define  NETBINDDES L"Bind Notify Process"

#define  NETBINDSUBLAYERNAME L"Bind Sublayer"
#define  NETBINDSUBLAYERDES L"Bind Sublayer Add"

#define  NETBINDFILTERNAME L"Bind Filter"
#define  NETBINDFILTERDES L"Bind Filter Bind"
// {0C9CE259-D84B-4008-AC17-F6158828D726}
DEFINE_GUID( CallOutTCPUDPConnectSublayer_V4, 0xc9ce259, 0xd84b, 0x4008, 0xac, 0x17, 0xf6, 0x15, 0x88, 0x28, 0xd7, 0x26 );
// {70095758-42A6-4B96-B84F-7C37091E2005}
DEFINE_GUID( CallOutTCPUDPBindSublayer_V4, 0x70095758, 0x42a6, 0x4b96, 0xb8, 0x4f, 0x7c, 0x37, 0x9, 0x1e, 0x20, 0x5 );

struct NetMonitorCallbackIDArry
{
	UINT32 uCallOutAddID;
	const GUID  *pLayerAddID;
	UINT64 uFilterAddID;
};

typedef std::map<PAGE_TYPE, IInterfacePresenter*> MAP_PagePresenter;
#define HANDLE_SUCCESS(h) ((( h ) != nullptr) && (( h ) != INVALID_HANDLE_VALUE ))

template<typename T> class ProcessLess //使数据不进行排序
{
public:
	bool operator ()( T y, T x )
	{
		return false;
	}
};
typedef std::multimap<DataType,PVOID, ProcessLess<DataType>> MULTIMAP_ProcessData;//进程的行为数据（注册表 文件 进程创建）
typedef std::map<UINT32, MULTIMAP_ProcessData> MAP_ProcessBehaviourData;//所有进程的行为
