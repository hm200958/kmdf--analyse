#include "StdAfx.h"
#include "GlobalFun.h"

#include <vector>
#include <string>
#include "UserDataDefine.h"

std::wstring Acsi2WideByte( const std::string& strascii )
{
	int widesize = MultiByteToWideChar( CP_ACP, 0, ( char* )strascii.c_str( ), -1, NULL, 0 );
	if ( widesize == ERROR_NO_UNICODE_TRANSLATION )
	{
		throw std::exception( "Invalid UTF-8 sequence." );
	}
	if ( widesize == 0 )
	{
		throw std::exception( "Error in conversion." );
	}
	std::vector <wchar_t> resultstring( widesize );
	int convresult = MultiByteToWideChar( CP_ACP, 0, ( char* )strascii.c_str( ), -1, &resultstring[ 0 ], widesize );


	if ( convresult != widesize )
	{
		throw std::exception( "char to widechar transfer failed" );
	}

	return std::wstring( &resultstring[ 0 ] );
}

std::wstring GetStringType( DataType eType )
{
	switch ( eType )
	{
		case DATA_TYPE_PROCESS_Create:
			{
				return std::wstring( _T( "创建进程" ) );
			}break;
		case DATA_TYPE_PROCESS_Exit:
			{
				return std::wstring( _T( "进程退出" ) );
			}break;
		case DATA_TYPE_REGISTER_CreateKey:
		case DATA_TYPE_REGISTER_CreateKeyEX:
			{
				return std::wstring( _T( "创建注册表键" ) );
			}break;
		case DATA_TYPE_REGISTER_DeleteKey:
			{
				return std::wstring( _T( "删除注册表键" ) );
			}break;
		case DATA_TYPE_REGISTER_SetValueKey:
			{
				return std::wstring( _T( "设置注册表键值" ) );
			}break;
		case DATA_TYPE_REGISTER_DeleteValueKey:
			{
				return std::wstring( _T( "删除注册表键值" ) );
			}break;
		case DATA_TYPE_REGISTER_QueryKey:
			{
				return std::wstring( _T( "查询注册表键" ) );
			}break;
		case DATA_TYPE_REGISTER_QueryValueKey:
			{
				return std::wstring( _T( "查询注册表键值" ) );
			}break;
		case DATA_TYPE_REGISTER_OpenKey:
		case DATA_TYPE_REGISTER_OpenKeyEx:
			{
				return std::wstring( _T( "打开注册表键" ) );
			}break;
		case DATA_TYPE_FILE_Create:
			{
				return std::wstring( _T( "创建文件" ) );
			}break;
		case DATA_TYPE_FILE_Create_Dir:
			{
				return std::wstring( _T( "创建目录" ) );
			}break;
		case DATA_TYPE_FILE_Write:
			{
				return std::wstring( _T( "写文件" ) );
			}break;
		case DATA_TYPE_FILE_Read:
			{
				return std::wstring( _T( "读文件" ) );
			}break;
		case DATA_TYPE_FILE_Set_Information:
			{
				return std::wstring( _T( "设置文件" ) );
			}break;
		case DATA_TYPE_FILE_Delete:
			{
				return std::wstring( _T( "删除文件" ) );
			}break;
		case DATA_TYPE_FILE_Rename:
			{
				return std::wstring( _T( "重命名文件" ) );
			}break;
		case DATA_TYPE_FILE_Open:
			{
				return std::wstring( _T( "打开文件" ) );
			}break;
		case DATA_TYPE_FILE_Open_Dir:
			{
				return std::wstring( _T( "打开目录" ) );
			}break;
		case DATA_TYPE_FILE_Close:
			{
				return std::wstring( _T( "关闭文件" ) );
			}break;
		case DATA_TYPE_NET_Bind:
			{
				return std::wstring( _T( "绑定监听" ) );
			}break;
		case DATA_TYPE_NET_Connect:
			{
				return std::wstring( _T( "对外连接" ) );
			}break;
		case DATA_TYPE_NET_DNS:
			{
				return std::wstring( _T( "域名解析" ) );
			}break;
		default:
			break;
	}

	return std::wstring( );
}
std::wstring GetRegString( RegDataType eType, PVOID pData )
{
	std::wstring str;
	if ( pData != nullptr )
	{
		switch ( eType )
		{
			case REG_SZ:
				{
					str = ( TCHAR* )pData;
				}break;
			case REG_EXPAND_SZ:
				{
					str = ( TCHAR* )pData;
				}break;
			case REG_BINARY:
				{

				}break;
			case REG_DWORD:
				{
					str = std::to_wstring( *( ( DWORD* )pData ) );
				}break;
			case REG_DWORD_BIG_ENDIAN:
				{
					str = std::to_wstring( *( ( DWORD* )pData ) );
				}break;
			case REG_LINK:
				{
					str = ( TCHAR* )pData;
				}break;
			case REG_MULTI_SZ:
				{
					str = ( TCHAR* )pData;
				}break;
			case REG_RESOURCE_LIST:
				{

				}break;
			case REG_FULL_RESOURCE_DESCRIPTOR:
				{

				}break;
			case REG_RESOURCE_REQUIREMENTS_LIST:
				{

				}break;
			case REG_QWORD:
				{
					str = std::to_wstring( *( ( INT64* )pData ) );
				}break;
			case REGDATATYPE_BasicClassInfo:
				{

				}break;
			case REGDATATYPE_NodeClassInfo:
				{

				}break;
			case REGDATATYPE_FullClassInfo:
				{

				}break;
			case REGDATATYPE_NameClassInfo:
				{

				}break;

			default:
				break;
		}
	}

	return str;
}
std::wstring GetNetString( NetMonitor *pData )
{
	CString strDec;
	if ( pData != nullptr )
	{
		switch ( pData->uProto )
		{
			case IPPROTO_TCP:
				{
					strDec = _T( "TCP" );
				}break;
			case IPPROTO_UDP:
				{
					strDec= _T( "UDP" );
				}break;
			case IPPROTO_ICMP:
				{
					strDec = _T( "ICMP" );
				}break;
			default:
				break;
		}

		switch ( pData->eType )
		{
			case DATA_TYPE_NET_Connect:
				{
					strDec.AppendFormat( _T( ":%s:%d--%s:%d" ), pData->SrcIP, pData->uSrcPort, pData->DesIP, pData->uDesPort );
				}break;
			case DATA_TYPE_NET_Bind:
				{
					strDec.AppendFormat( _T( ":%s:%d" ), pData->SrcIP, pData->uSrcPort );
				}break;
			case DATA_TYPE_NET_DNS:
				{
					strDec = Acsi2WideByte( std::string( (char *)pData->pDataBuff )).c_str();
				}break;
			default:
				break;
		}
	}
	return std::wstring(strDec.GetString( ));
}
std::wstring GetFileString( FileMonitor *pData )
{
	CString strDec;
	if ( pData != nullptr )
	{
		strDec = pData->strFileName;

		switch ( pData->eType )
		{
			case DATA_TYPE_FILE_Rename:
				{
					strDec.AppendFormat( _T( "-->%s" ), ( ( TCHAR* )pData->pDataBuff ) );
				}break;
			default:
				break;
		}
	}
	return std::wstring( strDec.GetString( ) );
}