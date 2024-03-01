#include "button.h"
#include "math/graphics/brush/brushes.h"

button::button(const std::wstring& text) :control()
{
	this->text = text;
}

void button::render(cveci2& position, const texture& renderTarget)
{
	miliseconds currentTime = getMiliseconds();
	fp part = (currentTime - lastClickTime) / 500.0;//500ms effect

	color c;
	if (part < 1) 
	{
		c = color::lerpcolor(clickColor, backGroundColor, part);
	}
	else 
	{
		c = backGroundColor;
	}
	renderTarget.fillRectangle(rectanglei2(position + borderSize, rect.size - (borderSize + borderSize)), solidColorBrush(c));

	renderBorder(position, renderTarget);
	renderText(position, renderTarget);
}

void button::click()
{
	lastClickTime = getMiliseconds();
	control::click();
}
