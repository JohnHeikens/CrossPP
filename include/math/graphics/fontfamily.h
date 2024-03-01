#include "texture.h"
#pragma once

//0x10 letters in a row
constexpr size_t asciiRowWidth = 0x10;
constexpr size_t asciiColumnHeight = 0x10;
//the amount of letters in an ascii charachter set
constexpr size_t asciiLetterCount = 0x100;

class fontFamily
{
public:
	//the texture has to have a transparent background with black letters.
	//Be careful: the actual texture will be modified!
	fontFamily(texture* texture, cbool& flipRows = true);
	void DrawLetter(cletter& l, cvec2& position, cfp& letterSize, const texture& renderTarget) const;
	void DrawLetter(cletter& l, cvec2& position, cfp& letterSize, const texture& renderTarget, const colorBrush& b) const;
	texture* tex;

	rectanglei2 letterRects[asciiLetterCount];
	
private:

};

