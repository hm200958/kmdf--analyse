#pragma once
#pragma pack(1)
// ×¢Òâ×Ö½ÚÐò
typedef struct _UDP_HEADER
{
	UINT16 usSourPort;
	UINT16 usDestPort;
	UINT16 usLength;
	UINT16 usCheckSum;
}UDP_HEADER, *PUDP_HEADER;

typedef struct _DNS_HEADER
{
	UINT16 usID;
	UINT16 usIdentigy;
	UINT16 usQuNum;
	UINT16 usReNum;
	UINT16 usRightNum;
	UINT16 usExNum;
}DNS_HEADER, *PDNS_HEADER;
#pragma pack()