#pragma once
#include "itemStack.h"
#include "application/control/form.h"
struct inventory;
struct inventoryForm : form
{
	fp scaleMultiplier = 1;
	itemStack itemHolding = itemStack();
	veci2 holdingMousePos = cveci2();
	inventory* inventoryToDisplay = nullptr;
	inventoryForm();

	virtual void linkUp(inventory* newInventoryToDisplay);
	virtual void unLink();
	virtual void mouseDown(cveci2& position, cmb& button) override;
	virtual void hover(cveci2& position) override;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	void updateScale();
};