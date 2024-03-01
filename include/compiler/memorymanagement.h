#pragma once
#include "GlobalFunctions.h"
#include <string>

enum types
{
	tvoid, tbool, tint, tfp,
	StandardTypeCount
};
const std::wstring varnames[StandardTypeCount]
{
	std::wstring(L"void"),
	std::wstring(L"bool"),
	std::wstring(L"int"),
	std::wstring(L"fp")
};
cint TypeSpace[StandardTypeCount]
{
	0,//void costs 0 bytes of memory because it is nothing.
	1,//boolean costs 1 byte of memory.
	4,//int costs 4 bytes of memory.
	8,//fp costs 8 bytes of memory.
};
inline types GetType(std::wstring typeN) 
{
	for (int i = 0; i < StandardTypeCount; i++) 
	{
		if (typeN == varnames[i])
		{
			return(types)i;
		}
	}
	return (types)-1;
}
struct variable
{
	inline static void copy(const variable* from, const  variable* to) 
	{
		switch (to->type)
		{
		case tbool:
			*(bool*)to->var = *(bool*)from->var;
			break;
		case tint:
			*(int*)to->var = *(int*)from->var;
			break;
		case tfp:
			*(fp*)to->var = *(fp*)from->var;
			break;
		}
	}
	variable();
	variable(types type, bool modifyable, std::wstring name = std::wstring(L""));
	variable(types type, void* var, bool modifyable, std::wstring name = std::wstring(L""));
	std::wstring name = std::wstring(L"");
	types type = tvoid;
	void* var = nullptr;//pointer
	bool modifyable = false;
};