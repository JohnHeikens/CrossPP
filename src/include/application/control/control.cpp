#include "control.h"
#include "math/graphics/brush/brushes.h"
#include "filesystem/filemanager.h"

// to make sure its initialized
// https://stackoverflow.com/questions/9092479/why-isnt-my-extern-variable-initialized-yet
theme &defaultTheme()
{
	static fontFamily privateDefaultFontFamily = fontFamily(new resolutionTexture(texture(stdPath(L"data") / L"ascii.png", true), cvec2(asciiRowWidth, asciiColumnHeight)));
	static font privateFont = font(privateDefaultFontFamily);
	static theme privateDefaultTheme = theme(&privateFont);
	return privateDefaultTheme;
}

void control::render(cveci2 &position, const texture &renderTarget)
{
	renderBackGround(position, renderTarget);
	renderBorder(position, renderTarget);
	if (text.length())
	{
		renderText(position, renderTarget);
	}
	renderChildren(position, renderTarget);
}

void control::renderBorder(cveci2 &position, const texture &renderTarget)
{
	renderTarget.fillRectangleBorders(crectanglei2(position, rect.size), borderSize, solidColorBrush(borderColor));
}

void control::renderBackGround(cveci2 &position, const texture &renderTarget)
{
	if (backGroundColor.a() > 0)
	{
		renderTarget.fillRectangle(rectanglei2(position + borderSize, rect.size - (borderSize + borderSize)), solidColorBrush(backGroundColor));
	}
}

void control::renderText(cveci2 &position, const texture &renderTarget)
{
	currentFont->DrawString(text, rectangle2(rectanglei2(position, rect.size).expanded(-borderSize)), renderTarget);
}

void control::renderChildren(cveci2 &position, const texture &renderTarget)
{
	for (control *element : children)
	{
		if (element->visible && element->rect.size.x > 0 && element->rect.size.y > 0)
		{
			element->render(position + element->rect.pos0, renderTarget);
		}
	}
}

control::control()
{
	children = fastList<control *>();
	this->currentFont = defaultTheme().font;
	this->borderSize = defaultTheme().borderSize;
	this->borderColor = defaultTheme().borderColor;
	this->backGroundColor = defaultTheme().backGroundColor;
}

void control::keyDown(cvk &keyCode)
{
	onKeyDown.invoke(keyEventArgs(*this, keyCode));
	if (keyCode == vk::Space || keyCode == vk::Enter)
	{
		click();
	}
	if (focusedChild)
	{
		focusedChild->keyDown(keyCode);
	}
}

void control::keyUp(cvk &keyCode)
{
	onKeyUp.invoke(keyEventArgs(*this, keyCode));
	// if (onKeyUp)
	//{
	//	onKeyUp(keyCode);
	// }
	if (focusedChild)
	{
		focusedChild->keyUp(keyCode);
	}
}

void control::keyPress(cvk &keyCode)
{
	onKeyPress.invoke(keyEventArgs(*this, keyCode));
	// if (onKeyPress)
	//{
	//	onKeyPress(keyCode);
	// }
	if (focusedChild)
	{
		focusedChild->keyPress(keyCode);
	}
}
void control::enterText(cuint &unicode)
{
	if (focusedChild)
	{
		focusedChild->enterText(unicode);
	}
}
void control::hover(cveci2 &position)
{
	// TODO: the button argument is not working
	onHover.invoke(mouseEventArgs(*this, position));
	// if (onHover)
	//{
	//	onHover(position);
	// }
	control *highest = getHighestChild(position);
	if (highest)
	{
		highest->hover(position - highest->rect.pos0);
	}
}
void control::mouseDown(cveci2 &position, cmb &button)
{
	control *highest = getHighestChild(position);
	onMouseDown.invoke(mouseButtonEventArgs(*this, position, button));

	if (highest != focusedChild)
	{
		focusChild(highest);
	}
	if (focusedChild)
	{
		focusedChild->mouseDown(position - focusedChild->rect.pos0, button);
	}
	else if (button == mb::Left)
	{
		click();
	}
}

void control::mouseUp(cveci2 &position, cmb &button)
{
	control *highest = getHighestChild(position);
	onMouseUp.invoke(mouseButtonEventArgs(*this, position, button));
	if (highest && (highest == focusedChild))
	{
		focusedChild->mouseUp(position - focusedChild->rect.pos0, button);
	}
}

void control::click()
{
	onClick.invoke(controlEventArgs(*this));
}

void control::scroll(cveci2 &position, cint &scrollDelta)
{
	onScroll.invoke(mouseWheelEventArgs(*this, position, scrollDelta));
	if (focusedChild)
	{
		focusedChild->scroll(position - focusedChild->rect.pos0, scrollDelta);
	}
}

void control::layout(crectanglei2 &newRect)
{
	rect = newRect;
}

void control::addChildren(std::initializer_list<control *> children)
{
	for (control *c : children)
	{
		c->parent = this;
	}
	this->children.push_back(children);
	this->children.update();
}

void control::focus()
{
	focused = true;
	onFocus.invoke(controlEventArgs(*this));
}

void control::lostFocus()
{
	focused = false;
	onLostFocus.invoke(controlEventArgs(*this));
	if (focusedChild != nullptr)
	{
		focusedChild->lostFocus();
		focusedChild = nullptr;
	}
}

void control::paste()
{
	if (focusedChild)
	{
		focusedChild->paste();
	}
}

void control::focusChild(control *newFocusedChild)
{
	if (focusedChild != newFocusedChild)
	{
		if (focusedChild)
		{
			focusedChild->lostFocus();
		}
		focusedChild = newFocusedChild;
		if (newFocusedChild)
		{
			newFocusedChild->focus();
		}
	}
}

void control::switchVisibleChild(control *newFocusedChild)
{
	if (focusedChild)
	{
		focusedChild->visible = false;
	}
	if (newFocusedChild)
	{
		newFocusedChild->visible = true;
	}
	focusChild(newFocusedChild);
}
control::~control()
{
	for (control *child : children)
	{

		delete child;
	}
}

control *control::getHighestChild(cveci2 &pos) const
{
	// loop through childs from back to front
	for (size_t index = children.size; index--;) // first comparison (index), then substraction (--)
	{
		control *currentChild = (children)[index];
		if (currentChild->visible && (currentChild->rect).contains(pos))
		{
			return (children)[index];
		}
	}
	return nullptr;
}