#include "area.h"
#include "standardarea.h"
#pragma once
struct compiler
{
	static void initialize();
	//std::vector<function*> Compile(std::wstring code);
	area* Compile(std::wstring code, area* a);
	area* Compile(std::vector<std::wstring> code, area* parent);
	std::vector<std::wstring> GetCodeLines(std::wstring code);
};
