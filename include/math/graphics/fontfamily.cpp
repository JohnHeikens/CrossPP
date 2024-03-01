#include "fontfamily.h"
#include "graphics.h"
#include "brush/brushes.h"

struct letterDrawer : public colorBrush
{
	const colorBrush* bottomBrush = nullptr;
	const colorBrush* fontBrush = nullptr;
	rectangle2 letterRect = vec2();
	rectangle2 maskBrushRect = vec2();
	virtual inline color getValue(cvec2& pos) const override
	{
		const color c = fontBrush->getValue((pos - letterRect.pos0) / letterRect.size * maskBrushRect.size + maskBrushRect.pos0);
		return colorMixer<>::getColor(c, bottomBrush->getValue(pos));
	}
};

fontFamily::fontFamily(texture* texture, cbool& flipRows)
{
	tex = texture;
	cvect2<size_t> texLetterSize = tex->size.x() / asciiRowWidth;
	if (flipRows)
	{
		for (size_t yRow0 = 0; yRow0 < asciiColumnHeight / 2; yRow0++)
		{
			size_t yRow1 = (asciiColumnHeight - yRow0 - 1);
			for (size_t aX = 0; aX < tex->size.x(); aX++)
			{
				for (size_t yIndex = 0; yIndex < texLetterSize.y(); yIndex++)
				{
					size_t aY0 = yRow0 * texLetterSize.y() + yIndex;
					size_t aY1 = yRow1 * texLetterSize.y() + yIndex;
					//swap colors
					std::swap(tex->getValueReferenceUnsafe(cvect2<size_t>(aX, aY0)), tex->getValueReferenceUnsafe(cvect2<size_t>(aX, aY1)));
				}
			}
		}
	}
}

void fontFamily::DrawLetter(cletter& l, cvec2& position, cfp& letterSize, const texture& renderTarget) const
{
	DrawLetter(l, position, letterSize, renderTarget, *tex);
}

void fontFamily::DrawLetter(cletter& l, cvec2& position, cfp& letterSize, const texture& renderTarget, const colorBrush& b) const
{
	cbyte letterIndex = (byte)l;
	cveci2 asciiOffset = cveci2(letterIndex % asciiRowWidth,//the x index of the letter texture
		letterIndex / asciiRowWidth);//the y index of the letter texture

	cveci2 texLetterSize = cveci2((int)(tex->size.x() / asciiRowWidth));
	cvec2 texOffset = asciiOffset * texLetterSize;

	letterDrawer drawer = letterDrawer();
	drawer.maskBrushRect = rectangle2(texOffset, vec2(texLetterSize));
	drawer.letterRect = rectangle2(position, vec2(letterSize));
	drawer.bottomBrush = &renderTarget;

	drawer.fontBrush = &b;
	renderTarget.fillRectangle(crectangle2(position.x(), position.y(), letterSize, letterSize), drawer);
}

