#include "math/font.h"
#include "brush/brushes.h"

vec2 font::DrawString(const std::wstring text, crectangle2& rect, const texture& renderTarget) const
{
	return DrawString(text, rect, MeasureStringOffset(rect, std::wstring(L"")), renderTarget);
}

vec2 font::DrawString(const std::wstring text, crectangle2& rect, vec2 offset, const texture& renderTarget) const
{
	colorBrush* brushToUse;
	if (fontBrush)
	{
		brushToUse = new colorMultiplier<texture, colorBrush>(*family->tex, *fontBrush);
	}
	else
	{
		brushToUse = family->tex;
	}
	if (text.length())
	{
		const letter* textptr = text.c_str();
		const letter* textendptr = textptr + text.length();

		//top left
		while (textptr < textendptr)
		{
			if (drawable(*textptr))
			{
				family->DrawLetter(*textptr, vec2(offset.x(), offset.y() - fontSize), fontSize, renderTarget, *brushToUse);
			}
			offset = MeasureLetterOffset(rect, offset, *textptr);
			textptr++;
		}
	}
	if (fontBrush)
	{
		delete brushToUse;
	}
	return offset;
}
