#include "GlobalFunctions.h"
#include <string>
#pragma once
struct INamable
{
	std::wstring name = std::wstring(L"");
	INamable(const std::wstring& name) :name(name) {}
};