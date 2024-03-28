#include "inventory.h"
#include "human.h"
void inventory::clickedOnItem(cmb& button, itemStack& stackHolding, uiSlotContainer* selectedSlotContainer, veci2 selectedSlot)
{
	if (stackHolding.count)
	{
		if (canAddStack(selectedSlotContainer, stackHolding))
		{
			if (button == mb::Right)
			{
				selectedSlotContainer->linkedContainer->getSlot(selectedSlot)->addStack(stackHolding, 1);
			}
			else
			{
				itemStack* occupyingSlot = selectedSlotContainer->linkedContainer->getSlot(selectedSlot);
				if (occupyingSlot->stackItemID == stackHolding.stackItemID && occupyingSlot->compare(stackHolding.stackItemID))
				{
					occupyingSlot->addStack(stackHolding);
				}
				else
				{
					//swap slots
					std::swap(*occupyingSlot, stackHolding);
				}
			}
		}
	}
	else
	{
		itemStack* s = selectedSlotContainer->getSlot(selectedSlot);
		stackHolding.addStack(*s);
	}
}

void inventory::drawExtraData(cmat3x3& transform, const texture& renderTarget)
{
}

inventory::~inventory()
{
	for (uiSlotContainer* container : containers)
	{
		delete container;
		container = nullptr;
	}
}

void inventory::mouseDown(cveci2& pixelPosition, cmb& button, itemStack& stackHolding)
{
	human* currentHuman = (human*)linkedPlayer;
	//check which inventory slot is clicked on
	//convert to 'hotbar slot space'
	veci2 selectedItemPosition;
	uiSlotContainer* selectedSlotContainer = getSlotContainer(pixelPosition, selectedItemPosition);
	if (selectedSlotContainer)
	{
		clickedOnItem(button, stackHolding, selectedSlotContainer, selectedItemPosition);
	}
	else if (stackHolding.count)
	{
		//outside of the slotcontainers
		//drop on the ground
		currentHuman->drop(stackHolding);
		stackHolding.clearData();
	}
}

void inventory::drawToolTips(cveci2& mouseTexturePosition, cveci2& mousePositionPixels, const texture& renderTarget)
{
	//check which inventory slot is hovered above
	//convert to 'hotbar slot space'
	veci2 selectedItemPosition;
	uiSlotContainer* container = getSlotContainer(mouseTexturePosition, selectedItemPosition);
	if (container)
	{
		itemStack* stack = container->linkedContainer->getSlot(selectedItemPosition);
		if (stack->count)
		{
			stack->drawToolTips(mousePositionPixels, renderTarget);
		}
	}
}

bool inventory::canAddStack(uiSlotContainer* containerToAddTo, itemStack& s)
{
	return true;
}

uiSlotContainer* inventory::getSlotContainer(cveci2& mousePositionPixels, veci2& slotPosition)
{
	//check which inventory slot is hovered above
	//convert to 'hotbar slot space'
	for (uiSlotContainer* const& container : containers)
	{
		if (container->linkedContainer->getSlot(mousePositionPixels, container->containerPixelPosition, inventorySpacing, inventoryItemDisplaySize, slotPosition))
		{
			return container;//no other slotcontainer will have items at the same place
		}
	}
	return nullptr;
}