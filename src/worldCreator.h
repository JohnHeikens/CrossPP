#pragma once
#include "application/control/form.h"
#include "application/control/label.h"
#include "application/control/TextBox.h"
#include "application/control/button.h"
#include "constants.h"
struct worldCreator : form
{
	world* worldCreating;
	worldCreator();
	label* worldNameLabel = new label(L"World Name");
	textBox* worldNameTextBox = new textBox();
	label* willBeSavedInLabel = new label();
	label* worldSeedLabel = new label(L"Seed For The World Generator");
	textBox* worldSeedTextBox = new textBox();
	button* createButton = new button(L"Create New World");
	button* cancelButton = new button(L"Cancel");
	button* cheatsButton = new button();
	virtual void mouseDown(cveci2& position, cvk& button) override;
	virtual void keyPress(cvk& key) override;
	virtual void layout(crectanglei2& newRect) override;
	void render(cveci2& position, const texture& renderTarget) override;
	void cancelWorldCreation();
	virtual ~worldCreator() override;
};
extern worldCreator* currentWorldCreator;