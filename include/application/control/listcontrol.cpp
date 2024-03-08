#include "listcontrol.h"

listControl::listControl(cint& sideSliderWidth) :control(),
sideSliderWidth(sideSliderWidth)
{
	addChildren({ sideSlider });
}

void listControl::mouseDown(cveci2& position, cvk& button)
{
	control::mouseDown(position, button);
	reOrganizeChildControls();
}

void listControl::reOrganizeChildControls()
{
	if (isDebugging && sideSliderWidth + childSize.x() > rect.w())
	{
		throw std::runtime_error("you left no room for the slider");
	}
	cveci2 arrangementRoom = rect.size - veci2(sideSlider->rect.w(), 0);
	cint arrangementWidth = childSize.x() > 0 ? math::maximum((int)floor(arrangementRoom.x() / childSize.x()), 1) : 0;

	cint totalSpaceHeight = (int)(children.size - 1) * childSize.y();
	cint invisibleSpaceHeight = math::maximum(totalSpaceHeight - arrangementRoom.y(), 0);
	cint controlOffset = math::lerp(invisibleSpaceHeight, 0, sideSlider->value);
	veci2 arrangementOffset = veci2(0, -1);
	for (int i = 0; i < children.size; i++) 
	{
		control* c = (children)[i];
		if (c != sideSlider)
		{
			c->layout(crectanglei2(arrangementOffset * childSize + cveci2(0, rect.h() + controlOffset), childSize));
			arrangementOffset.x()++;
			if (arrangementOffset.x() >= arrangementWidth)
			{
				arrangementOffset.x() = 0;
				arrangementOffset.y()--;
			}
		}
	}
}
control* listControl::getSelectedChild()
{
	control* c = focusedChild;
	return c == sideSlider ? nullptr : c;
}

void listControl::layout(crectanglei2& newRect)
{
	control::layout(newRect);
	crectanglei2 sideSliderRect = crectanglei2(rect.w() - sideSliderWidth, 0, sideSliderWidth, rect.h());
	sideSlider->layout(sideSliderRect);
	reOrganizeChildControls();
}
