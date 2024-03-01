#pragma once
#include "rectangularSlotContainer.h"
#include "include/interface/idestructable.h"
#include "include/filesystem/jsonReader.h"
struct recipe :IDestructable
{
	//returns substraction positions
	virtual bool verifyPattern(rectangularSlotContainer* container);
	virtual ~recipe() override;
	itemStack result;
};
void substractOneOfEachSlot(rectangularSlotContainer* container);
recipe* getCraftingRecipe(rectangularSlotContainer* const& container);
extern std::vector<recipe*> craftingRecipes;
template<typename recipeType>
recipeType* findRecipe(std::vector<recipeType*> recipeList, rectangularSlotContainer* const& container)
{
	for (recipeType* r : recipeList)
	{
		if (r->verifyPattern(container))
		{
			return r;
		}
	}
	return nullptr;
}
void readRecipe(const jsonContainer& recipeDescription);
