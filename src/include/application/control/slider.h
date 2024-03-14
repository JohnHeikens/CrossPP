#include "control.h"
#include "button.h"
#pragma once

struct slider :public control
{
	slider(cfp& minValue, cfp& maxValue, cfp& value, cfp& step = 1);
	button* decreaseButton = new button();
	button* increaseButton = new button();
	button* dragButton = new button();
	void (*onDragCompleted)() = nullptr;
	fp value = 0;
	fp minValue, maxValue;
	fp step = 1;

	//clamped
	void rePositionDragButton();
	void setValue(cfp& newValue);

	virtual void mouseDown(cveci2& position, cvk& button) override;
	virtual void dragCompleted();
	virtual void layout(crectanglei2& newRect) override;
	color checkMarkColor = colorPalette::white;
};