#include "application/control/form.h"
#include <application/control/button.h>
#include <application/control/TextBox.h>
#include "math/rectangletn.h"
#pragma once

bool checkAccountName(const std::wstring& n);

struct accountEditor : form {
	textBox* playerNameTextBox = new textBox();
	button* saveButton = new button(L"Save");

	accountEditor();
	virtual void layout(crectanglei2& newRect) override;
	virtual void mouseDown(cveci2& position, cvk& button) override;
	virtual void renderBackGround(cveci2& position, const texture& renderTarget) override;
};
extern accountEditor* currentAccountEditor;