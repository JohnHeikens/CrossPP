#include "craftableSlotContainer.h"
#include "recipe.h"
#include "human.h"
#include "dimension.h"
void craftableSlotContainer::substractCraftingIngredients()
{
	for (uiSlotContainer* container : craftingInputContainers)
	{
		substractOneOfEachSlot(container->linkedContainer);
	}
}

void craftableSlotContainer::recalculateOutputSlot()
{
	human* currentHuman = (human*)linkedPlayer;
	itemStack newRecipeResult = calculateRecipeResult();
	if (currentRecipeResult.count && (!currentRecipeResult.compare(newRecipeResult)))
	{
		//the player still has some items left from the last crafting session
		int outputCount = craftingOutputSlot->linkedContainer->slots[0].count;
		if (outputCount < currentRecipeResult.count)
		{
			craftingOutputSlot->linkedContainer->transfer((human*)linkedPlayer, linkedPlayer->dimensionIn, currentHuman->getDropPosition());
		}
	}
	currentRecipeResult.clearData();
	craftingOutputSlot->linkedContainer->clearData();

	currentRecipeResult = newRecipeResult;

	//update result slot
	craftingOutputSlot->linkedContainer->slots[0] = currentRecipeResult.count ? currentRecipeResult : itemStack();
}

void craftableSlotContainer::clickedOnItem(cmb& button, itemStack& stackHolding, uiSlotContainer* selectedSlotContainer, veci2 selectedSlot)
{
	if (selectedSlotContainer == craftingOutputSlot)
	{
		if (currentRecipeResult.count)
		{
			itemStack* s = selectedSlotContainer->getSlot(selectedSlot);
			cint previousOutputSlotCount = s->count;
			if (stackHolding.count == 0 || s->compare(stackHolding))
			{
				//add crafting result
				stackHolding.addStack(*s);

				if (previousOutputSlotCount == currentRecipeResult.count && s->count < currentRecipeResult.count)
				{
					substractCraftingIngredients();
				}

				if (s->count == 0) //there are no items left in the crafting output slot
				{
					recalculateOutputSlot();
				}
			}
		}
	}
	else
	{
		inventory::clickedOnItem(button, stackHolding, selectedSlotContainer, selectedSlot);
		if (std::find(craftingInputContainers.begin(), craftingInputContainers.end(), selectedSlotContainer) != craftingInputContainers.end())
		{
			recalculateOutputSlot();
		}
	}
}