#pragma once
#include "application/control/form.h"
#include <application/control/TextBox.h>
#include <application/control/button.h>
#include <application/control/label.h>
struct serverEditor :form {
	serverEditor();
	label* nameLabel = new label(L"Name...");
	textBox* nameTextBox = new textBox();
	label* ipAddressLabel = new label(L"ipaddress:port");
	textBox* ipAddressTextBox = new textBox();
	button* doneButton = new button(L"Done");
	virtual void layout(crectanglei2& newRect) override;
	virtual void keyPress(cvk& key) override;
	virtual void mouseDown(cveci2& position, cmb& button);
};
extern serverEditor* currentServerEditor;