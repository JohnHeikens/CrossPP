#include "array/fastlist.h"
#include "math/theme.h"
#include "event/eventhandler.h"
#include "event/eventargs.h"
#include <initializer_list>
#include <sfml/Window.hpp>

struct control;

typedef eventArgs<control> controlEventArgs;
typedef eventHandler<controlEventArgs> controlEventHandler;

struct mouseEventArgs : controlEventArgs 
{
	veci2 position = veci2();
	mouseEventArgs(control& sencder, cveci2& position) : controlEventArgs(sender), position(position) 
	{

	}
};
struct mouseButtonEventArgs : mouseEventArgs{
	mb button;
	mouseButtonEventArgs(control& sencder, cveci2& position, cmb& button) : mouseEventArgs(sender, position), button(button) 
	{

	}
};
typedef eventHandler<mouseEventArgs> mouseEventHandler;

struct mouseWheelEventArgs : mouseEventArgs
{
	int scrollDelta = 0;
	mouseWheelEventArgs(control& sencder, cveci2& position, cint& scrollDelta):mouseEventArgs(sender, position), scrollDelta(scrollDelta)
	{
	
	}
};
typedef eventHandler<mouseWheelEventArgs> mouseWheelEventHandler;

struct keyEventArgs : controlEventArgs
{
	vk keyCode;
	keyEventArgs(control& sender, cvk& keyCode) : controlEventArgs(sender), keyCode(keyCode)
	{

	}
};
typedef eventHandler<keyEventArgs> keyEventHandler;

struct layoutEventArgs : controlEventArgs
{
	rectanglei2 newRect = rectanglei2();

	layoutEventArgs(control& sender, crectanglei2& newRect) : controlEventArgs(sender), newRect(newRect){}
};
typedef eventHandler<layoutEventArgs> layoutEventHandler;

#pragma once
struct control :IDestructable
{
public:
	//pointer functions

	keyEventHandler onKeyDown = keyEventHandler();
	keyEventHandler onKeyPress = keyEventHandler();
	keyEventHandler onKeyUp = keyEventHandler();

	controlEventHandler onClick = controlEventHandler();

	mouseWheelEventHandler onScroll = mouseWheelEventHandler();

	mouseEventHandler onMouseDown = mouseEventHandler();
	mouseEventHandler onHover = mouseEventHandler();
	mouseEventHandler onMouseUp = mouseEventHandler();

	controlEventHandler onFocus = controlEventHandler();
	controlEventHandler onLostFocus = controlEventHandler();

	//void (*onFocusChild)(control* newFocusedChild) = nullptr;
	//void (*onLayout)(crectanglei2& newRect) = nullptr;
	//void (*onPaste)() = nullptr;

	control();

	//function invokers
	//position includes the position of this element
	virtual void render(cveci2& position, const texture& renderTarget);
	virtual void renderBorder(cveci2& position, const texture& renderTarget);
	virtual void renderBackGround(cveci2& position, const texture& renderTarget);
	virtual void renderText(cveci2& position, const texture& renderTarget);
	virtual void renderChildren(cveci2& position, const texture& renderTarget);

	virtual void keyDown(cvk& keyCode);
	virtual void keyUp(cvk& keyCode);
	virtual void keyPress(cvk& keyCode);
	virtual void enterText(cuint& keyCode);

	virtual void hover(cveci2& position);
	virtual void mouseDown(cveci2& position, cmb& button);
	virtual void mouseUp(cveci2& position, cmb& button);
	virtual void click();
	virtual void scroll(cveci2& position, cint& scrollDelta);
	virtual void layout(crectanglei2& newRect);
	void addChildren(std::initializer_list<control*> children);

	template<typename childListType>
	void layoutTableCentered(const childListType& children, cveci2& childSize = buttonSize, cint& margin = buttonMargin) {
		cint& offsetStep = childSize.y() + margin;
		//using end - begin instead of size() because fastlist doesn't have size()
		cveci2& tablePos = rect.rectPos0Centered(cveci2(childSize.x(), childSize.y() + offsetStep * ((int)(children.end() - children.begin()) - 1)));
		rectanglei2 currentChildRect = rectanglei2(tablePos, childSize);
		for (auto c : children) {
			c->layout(currentChildRect);
			currentChildRect.y() += offsetStep;
		}
	}

	//called when this control gains focus
	virtual void focus();
	//called when this control looses focus
	virtual void lostFocus();

	virtual void paste();

	//called when a child gains focus
	void focusChild(control* newFocusedChild);

	void switchVisibleChild(control* newFocusedChild);

	virtual ~control() override;
	constexpr bool hasBeenLayout() const {
		return rect.size.x();
	}

	//raycasts from top to bottom to check which child is hit
	control* getHighestChild(cveci2& pos) const;

	control* parent = nullptr;

	bool visible = true;
	//if this control has focus, regardless of one of its child controls having focus
	bool focused = false;

	rectanglei2 rect = rectanglei2();

	//pointer to the child that has the focus
	control* focusedChild = nullptr;
	fastList<control*> children = fastList<control*>();

	baseFont* currentFont = nullptr;

	std::wstring text = std::wstring(L"");
	verticalAlignment textVerticalAlignment = verticalAlignment::top;
	horizontalAlignment textHorizontalAlignment = horizontalAlignment::left;
	int borderSize = 2;

	color backGroundColor = colorPalette::black;
	color borderColor = colorPalette::white;
};

theme& defaultTheme();
