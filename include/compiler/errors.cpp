#include "errors.h"
std::wstring errormessages[(fsize_t)errortypes::errortypecount]//DO NOT FORGET THE COMMA (,)
{
	std::wstring(L"no operator found"),
	std::wstring(L"calculation has a length of zero"),
	std::wstring(L"no closing parenthesis \"}\" found"),
	std::wstring(L"no closing square bracket \"]\" found"),
	std::wstring(L"no closing brace \")\" found"),
	std::wstring(L"something is typed after the closing bracket"),
	std::wstring(L"this line is too short"),
	std::wstring(L"could not find the function you specified"),
	std::wstring(L"not all indexes of this array are filled"),
	std::wstring(L"variable must be a modifyable l-value"),
	std::wstring(L"could not find an appropriate function overload"),
};
