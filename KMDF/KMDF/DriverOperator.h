#pragma once
#include "Define.h"
class CDriverMem
{
public:
	CDriverMem() { }
	virtual ~CDriverMem() { }
	void *operator new(size_t size)
	{
		KdPrint(("operator new %d\n", size));
		return ExAllocatePoolWithTag(NonPagedPool, size,ALLOCTAG);
	}
	void operator delete(void *p)
	{
		ExFreePoolWithTag(p,ALLOCTAG);
	}
};
