#include "math/graphics/brush/font/baseFont.h"
#include "math/collisions.h"
#include "colors.h"
#include <string>
#include "math/graphics/texture.h"
#include "math/graphics/color/color.h"
#include "math/graphics/brush/brushes.h"
#include "math/rectangletn.h"
#include "math/vectn.h"
#include "GlobalFunctions.h"
#pragma once

extern fontFamily* currentMinecraftFontFamily;
struct minecraftFont : public baseFont {
	minecraftFont(cfp& fontSize = defaultFontSize) : baseFont(*currentMinecraftFontFamily, fontSize) {}
	inline virtual vec2 DrawString(const std::wstring text, crectangle2& rect, cvec2& offset, const texture& renderTarget, const std::optional<mat3x3>& matrix = std::nullopt) const override
	{
		color currentTextColor = colorPalette::white;
		bool obfuscated = false;
		int obfuscateLength = 0;

		const std::wstring obfuscateLetters = std::wstring(L"!@#$%^&*()}{[]\\/.,;'`~");
		const auto& currentTextBrush = brushes::white;

		vec2 currentOffset = offset;

		//color ranges
		for (int i = 0; i < text.size(); i++)
		{
			letter currentLetter = text[i];

			//const auto& currentTextBrush = colorMultiplier<texture, solidColorBrush>(*creditsFont.family->tex, brushes::white);

			if (currentLetter == L'�')
			{
				if (obfuscated)
				{
					obfuscated = false;
					if (obfuscateLength == 0)//pick a random length
					{
						obfuscateLength = rand(currentRandom, 0x8, 0xc);
					}

					//creditsFont.fontBrush = &currentTextBrush;
					for (int i = 0; i < obfuscateLength; i++)
					{
						currentLetter = obfuscateLetters[randIndex(currentRandom, obfuscateLetters.size())];
						if (collides2d(rect, crectangle2(currentOffset, cvec2(fontSize))))
						{
							//mat3x3 t = mat3x3::cross(transform, mat3x3::fromRectToRect(crectangle2()));
							if (currentTextColor == colorPalette::white)
							{
								family.DrawLetter(currentLetter, currentOffset, fontSize, renderTarget, *family.tex, matrix);
							}
							else
							{
								family.DrawLetter(currentLetter, currentOffset, fontSize, renderTarget, colorMultiplier<resolutionTexture, solidColorBrush>(*family.tex, currentTextBrush), matrix);
							}
						}
						currentOffset = MeasureLetterOffset(rect, currentOffset, currentLetter);
					}
				}

				letter formatCodeLetter = text[i + 1];
				int colorCode;
				if (convertToInt(std::wstring(1, formatCodeLetter), colorCode, 0x10))
				{
					currentTextColor = rgbFormatColors[colorCode];
				}
				else if (formatCodeLetter == L'k')
				{
					obfuscated = true;
					obfuscateLength = 0;
				}
				else if (formatCodeLetter == L'r')
				{
					//reset
					currentTextColor = colorPalette::white;
				}
				else
				{
					handleError(std::wstring(L"invalid sign behind \"�\""));
				}
				i++;
			}
			else
			{
				if (obfuscated)
				{
					obfuscateLength++;
				}
				else if (baseFont::drawable(currentLetter))
				{
					if (currentTextColor == colorPalette::white)
					{
						family.DrawLetter(currentLetter, currentOffset, fontSize, renderTarget, *family.tex, matrix);
					}
					else
					{
						family.DrawLetter(currentLetter, currentOffset, fontSize, renderTarget, colorMultiplier<resolutionTexture, solidColorBrush>(*family.tex, currentTextBrush), matrix);
					}
				}
				currentOffset = MeasureLetterOffset(rect, currentOffset, currentLetter);
			}
		}
		return currentOffset;
	}
	using baseFont::DrawString;
};
extern minecraftFont* currentMinecraftFont;