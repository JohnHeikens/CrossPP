#pragma once
#include "control.h"
struct textBox :public control {
public:
	textBox();
	size_t cursorIndex = 0;
	bool capsLock = false;
	bool shift = false;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void enterText(cuint& uniCode) override;
	virtual void keyDown(cvk& keyCode) override;
	//virtual void keyUp(cvk& keyCode) override;
	virtual void lostFocus() override;
	virtual void paste() override;
	virtual void mouseDown(cveci2& position, cmb& button) override;
	void checkCursorIndex();
};