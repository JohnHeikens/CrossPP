#include "GlobalFunctions.h"
#include <string>
//#include <Windows.h>
#pragma once
//https://en.wikipedia.org/wiki/Bracket
enum class errortypes
{
	nooperator,
	calclengthzero,
	noclosingparenthese,//{}
	noclosingsquarebracket,//[]
	noclosingbrace,//()
	typedafterclosingbracket,
	linetooshort,
	couldnotfindfunction,
	notallindexesinitialized,
	mustbemodifyable,
	noavailableoverload,
	errortypecount,
};
extern std::wstring errormessages[(fsize_t)errortypes::errortypecount];
inline void ShowCompilerError(errortypes errortype, std::wstring code)
{
	//if (MessageBox(NULL, (
	//	errormessages[(fsize_t)errortype] + std::wstring(L"\ncode:\n") + code + std::wstring(L"\nerrorcode:") + std::to_wstring((fsize_t)errortype)
	//	+ std::wstring(L"\ncontinue compiling?")).c_str(), L"Error", MB_YESNO) == IDYES)
	//{
//
	//}
	//else
	//{
	//	throw 0;
	//}
}