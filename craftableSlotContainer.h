#pragma once
#include "inventory.h"
struct craftableSlotContainer :inventory
{
	uiSlotContainer* craftingOutputSlot = nullptr;

	itemStack currentRecipeResult = itemStack();

	std::vector<uiSlotContainer*> craftingInputContainers = std::vector<uiSlotContainer*>();

	virtual itemStack calculateRecipeResult() = 0;
	virtual void substractCraftingIngredients();
	void recalculateOutputSlot();
	virtual void clickedOnItem(cvk& button, itemStack& stackHolding, uiSlotContainer* selectedSlotContainer, veci2 selectedSlot) override;
};