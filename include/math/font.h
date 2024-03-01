#pragma once
#include "graphics/fontfamily.h"
#include "graphics/brush/brushes.h"

constexpr fp defaultFontSize = 0x10;
struct font 
{
	fontFamily* family = nullptr;
	fp fontSize = defaultFontSize;
	const colorBrush* fontBrush = nullptr;

	//this includes the font size!
	fp lineSpacing = 1;

	font(fontFamily* family, fp fontSize = 0x10, const colorBrush* fontBrush = &brushes::white) :
		family(family), fontSize(fontSize), fontBrush(fontBrush){}

	constexpr fp getLineHeight() const
	{
		return fontSize + lineSpacing;
	}

	constexpr static bool drawable(const letter& l) 
	{
		return l != L'\n' && l != L'\t' && l != L'\r';
	}

	inline vec2 MeasureLetterOffset(crectangle2& bounds, cvec2 position, const letter& l) const
	{
		if (l == L'\n') 
		{
			return vec2(bounds.x(), position.y() - getLineHeight());
		}
		else 
		{
			cfp& letterWidth = fontSize;
			cfp& newX = position.x() + letterWidth;
			if (position.x() > bounds.w() + bounds.x()) 
			{
				//new line
				return vec2(bounds.x() + letterWidth, position.y() - getLineHeight());
			}
			else 
			{
				return vec2(newX, position.y());
			}
		}

	}
	inline vec2 MeasureStringOffset(crectangle2& bounds, const std::wstring& text) const
	{
		//start at top left
		vec2 offset = vec2(bounds.pos0.x(), bounds.pos0.y() + bounds.h());
		for (int i = 0; i < text.size(); i++) 
		{
			offset = MeasureLetterOffset(bounds, offset, text[i]);
		}
		return offset;
	}
	inline vec2 measureStringSize(cvec2& maximumSize, const std::wstring& text) 
	{
		//start at top left
		fp widestX = 0;
		vec2 offset = vec2();
		crectangle2 maximalBounds = crectangle2(0, maximumSize.y(), maximumSize.x(), maximumSize.y());
		for (int i = 0; i < text.size(); i++)
		{
			offset = MeasureLetterOffset(maximalBounds, offset, text[i]);
			widestX = math::maximum(offset.x(), widestX);
		}
		return vec2(widestX, -offset.y() + getLineHeight());
	}
	//returns the position for the next letter like measurestring
	vec2 DrawString(const std::wstring text, crectangle2& rect, const texture& renderTarget) const;
	vec2 DrawString(const std::wstring text, crectangle2& rect, vec2 offset, const texture& renderTarget) const;
};