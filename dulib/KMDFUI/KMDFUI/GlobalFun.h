#include <vector>
#include <string>
#include "UserDataDefine.h"

std::wstring GetStringType( DataType eType );
std::wstring GetRegString( RegDataType eType, PVOID pData );
std::wstring GetNetString( NetMonitor* pData );
std::wstring GetFileString( FileMonitor *pData );
std::wstring Acsi2WideByte( const std::string& strascii );