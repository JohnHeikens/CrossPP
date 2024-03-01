#pragma once
#include "include/application/control/form.h"
#include "include/application/control/button.h"
#include "include/application/control/picturebox.h"
#include "include/application/control/label.h"
struct mainMenu : public form
{
	mainMenu();
	button* playOfflineButton = new button(L"Play Offline");
	button* playOnlineButton = new button(L"Play Online");
	pictureBox* backgroundPicture = nullptr;
	label* gameNameLabel = nullptr;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void mouseDown(cveci2& position, cvk& button) override;
	virtual void layout(crectanglei2& newRect) override;
	virtual void focus() override;
	virtual void keyDown(cvk& key) override;
};
extern mainMenu* currentMainMenu;