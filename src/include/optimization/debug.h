#pragma once
#include <string>
//#include <Windows.h>

//writes to output window
//inline void output(std::wstring s)
//{
//	OutputDebugString(s.c_str());
//}

#ifdef _DEBUG
//constexpr bool isDebugging = false;//for debugging purposes
constexpr bool isDebugging = true;
#else
constexpr bool isDebugging = false;
#endif // _DEBUG
