#include <string>
#include "include/math/graphics/color/color.h"
#include "colorID.h"
#pragma once
const std::wstring colorNames[(int)colorID::count]
{
	std::wstring(L"black"),
	std::wstring(L"white"),
	std::wstring(L"red"),
	std::wstring(L"orange"),
	std::wstring(L"pink"),
	std::wstring(L"yellow"),
	std::wstring(L"lime"),
	std::wstring(L"green"),
	std::wstring(L"light_blue"),
	std::wstring(L"cyan"),
	std::wstring(L"blue"),
	std::wstring(L"magenta"),
	std::wstring(L"purple"),
	std::wstring(L"brown"),
	std::wstring(L"gray"),
	std::wstring(L"light_gray"),
};

constexpr color rgbColorValues[(int)colorID::count]
{
	color::FromHex(0x1D1D21),
	color::FromHex(0xF9FFFE),
	color::FromHex(0xB02E26),
	color::FromHex(0xF9801D),
	color::FromHex(0xF38BAA),
	color::FromHex(0xFED83D),
	color::FromHex(0x80C71F),
	color::FromHex(0x5E7C16),
	color::FromHex(0x3AB3DA),
	color::FromHex(0x169C9C),
	color::FromHex(0x3C44AA),
	color::FromHex(0xC74EBD),
	color::FromHex(0x8932B8),
	color::FromHex(0x835432),
	color::FromHex(0x474F52),
	color::FromHex(0x9D9D97),
};

//§# <-heximal value
constexpr color rgbFormatColors[(int)colorID::count]
{
	color::FromHex(0xAA0000),
	color::FromHex(0x000000),
	color::FromHex(0x0000AA),
	color::FromHex(0x00AA00),
	color::FromHex(0x00AAAA),
	color::FromHex(0xAA00AA),
	color::FromHex(0xFFAA00),
	color::FromHex(0xAAAAAA),
	color::FromHex(0x555555),
	color::FromHex(0x5555FF),
	color::FromHex(0x55FF55),
	color::FromHex(0x55FFFF),
	color::FromHex(0xFF5555),
	color::FromHex(0xFF55FF),
	color::FromHex(0xFFFF55),
	color::FromHex(0xFFFFFF),
};
