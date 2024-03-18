#pragma once
#include "constants.h"
#include "structureBlockLoadModeUI.h"
#include "structureBlockSaveModeUI.h"
struct structureBlockUI : form
{
	human* currentPlayableCharachter = nullptr;
	structureBlockData* temporaryStructureBlockData = nullptr;
	label* structureBlockLabel = new label();
	label* nameLabel = new label();
	textBox* nameTextbox = new textBox();

	label* modeLabel = new label();
	button* modeButton = new button();

	button* doneButton = new button();
	button* cancelButton = new button();

	bool showInvisibleBlocks = false;

	structureBlockSaveModeUI* saveUI = new structureBlockSaveModeUI();
	structureBlockLoadModeUI* loadUI = new structureBlockLoadModeUI();
	//corner mode has no ui

	structureBlockUI();
	void serializeWithTemporaryData(cbool& write);
	virtual void layout(crectanglei2& rect) override;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void mouseDown(cveci2& position, cmb& button) override;
};