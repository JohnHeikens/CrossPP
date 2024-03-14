#pragma once
#include "application/control/form.h"
#include "application/control/listcontrol.h"

struct gameSelector : form
{
	//show a list with save files
	std::wstring gameTypeName = L"";
	gameSelector();
	listControl* gameListControl = new listControl(0x10);
	button* openButton = new button();//texts will be set by children
	button* newButton = new button();
	button* deleteButton = new button(L"Delete");
	button* refreshButton = new button(L"Refresh");
	virtual void mouseDown(cveci2& position, cvk& button) override;
	void refresh();
	virtual void keyPress(cvk& key) override;
	virtual void layout(crectanglei2& newRect) override;
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void openGame(const control* const c) const = 0;
	virtual void deleteGame(const control* const c) = 0;
	virtual void addGame() = 0;
	virtual void addGameControls() = 0;
};