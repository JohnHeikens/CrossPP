#pragma once
#include "math/graphics/brush/font/font.h"
constexpr int defaultBorderSize = 1;
constexpr color defaultBorderColor = colorPalette::gray;

struct theme
{
	theme(baseFont* font = nullptr, int borderSize = defaultBorderSize, color backGroundColor = colorPalette::black, color borderColor = defaultBorderColor) :
		font(font), borderSize(borderSize), backGroundColor(backGroundColor), borderColor(borderColor) {}
	int borderSize = 1;
	color backGroundColor = colorPalette::black;
	color borderColor = defaultBorderColor;
	baseFont* font = nullptr;
};