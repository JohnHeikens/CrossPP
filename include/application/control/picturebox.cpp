#include "picturebox.h"

pictureBox::pictureBox(texture* const& backGroundImage)
{
	this->backGroundImage = backGroundImage;
}

void pictureBox::render(cveci2& position, const texture& renderTarget)
{
	if (backGroundImage)
	{
		fillTransformedTexture(rectangle2(cvec2(position + borderSize), cvec2(rect.size - borderSize * 2)), *backGroundImage, renderTarget);
	}
	renderBorder(position, renderTarget);
	renderText(position, renderTarget);
	renderChildren(position, renderTarget);
}