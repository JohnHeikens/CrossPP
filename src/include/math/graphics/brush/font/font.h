#pragma once
#include "baseFont.h"
#include <string>
#include "GlobalFunctions.h"
#include "fontfamily.h"
#include "math/graphics/texture.h"
#include "math/rectangletn.h"
#include "math/vectn.h"

template<typename fontBrushType = solidColorBrush>
struct font : baseFont
{
	const fontBrushType* fontBrush = nullptr;

	font(fontFamily& family, fp fontSize = 0x10, const fontBrushType* fontBrush = &brushes::white) :
		baseFont(family, fontSize), fontBrush(fontBrush) {}
	font(const baseFont& f, const fontBrushType* fontBrush = &brushes::white) :
		baseFont(f.family, f.fontSize), fontBrush(fontBrush) {}

	//returns the position for the next letter like measurestring
	inline vec2 DrawString(const std::wstring text, crectangle2& rect, const texture& renderTarget) const
	{
		return baseFont::DrawString(text, rect, renderTarget, *fontBrush);
	}
	vec2 DrawString(const std::wstring text, crectangle2& rect, vec2 offset, const texture& renderTarget) const {
		return baseFont::DrawString(text, rect, offset, renderTarget, *fontBrush);
	}
};