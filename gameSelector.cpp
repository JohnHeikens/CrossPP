#include "gameSelector.h"
#include "world.h"
#include "saveFileControl.h"
#include "gameControl.h"
#include "gameForm.h"
#include "keyID.h"
#include "include/filesystem/filemanager.h"
#include "folderList.h"
#include "mainMenu.h"
gameSelector::gameSelector() : form()
{
	//add gamelistcontrol first, so it will be at the bottom of the controls
	addChildren({ gameListControl, openButton, newButton, deleteButton, refreshButton });
}

void gameSelector::mouseDown(cveci2& position, cvk& button)
{
	control* highestChild = getHighestChild(position);
	if (highestChild == openButton)
	{
		//open the selected world
		control* c = gameListControl->getSelectedChild();
		if (c)
		{
			openGame(c);
		}
	}
	else if (highestChild == newButton)
	{
		//create a new world
		visible = false;
		addGame();
	}
	else if (highestChild == refreshButton)
	{
		refresh();
	}
	else if (highestChild == deleteButton)
	{
		//delete the selected world
		control* c = gameListControl->getSelectedChild();
		if (c)
		{
			//ask for confirmation
			const std::wstring& messageString = L"Are You Sure You Want To Delete This " + gameTypeName + L"?";
			if (MessageBox(NULL, messageString.c_str(), gameName.c_str(), MB_OKCANCEL | MB_ICONWARNING) == IDOK)
			{
				deleteGame(c);
				refresh();
			}
		}
	}
	form::mouseDown(position, button);
}

void gameSelector::refresh()
{
	for (int i = 0; i < gameListControl->children->size; i++)
	{
		control* c = (*gameListControl->children)[i];
		if (c != gameListControl->sideSlider)
		{
			delete c;
			gameListControl->children->erase(i);
		}
	}
	addGameControls();
	gameListControl->children->update();
	gameListControl->focusedChild = nullptr;
	if (hasBeenLayout()) {
		gameListControl->reOrganizeChildControls();
	}
}

void gameSelector::keyPress(cvk& key)
{
	if (key == (cvk)keyID::escape)
	{
		visible = false;
		currentMainMenu->visible = true;
		mainForm->focusedChild = currentMainMenu;
	}
}

void gameSelector::layout(crectanglei2& newRect)
{
	form::layout(newRect);
	openButton->layout(crectanglei2(0, 0, rect.w() / 2, (int)defaultTheme().font->fontSize + defaultTheme().borderSize * 2));
	newButton->layout(crectanglei2(openButton->rect.w(), 0, rect.w() - openButton->rect.w(), openButton->rect.h()));
	deleteButton->layout(crectanglei2(0, openButton->rect.h(), rect.w() / 2, (int)defaultTheme().font->fontSize + defaultTheme().borderSize * 2));
	refreshButton->layout(crectanglei2(openButton->rect.w(), openButton->rect.h(), rect.w() - openButton->rect.w(), openButton->rect.h()));

	cint optionSpace = openButton->rect.h() * 2;

	gameListControl->childSize = cveci2(rect.w() / 2, rect.h() / 5);
	crectanglei2 saveFolderListControlRect = crectanglei2((rect.w() - gameListControl->childSize.x()) / 2, optionSpace, gameListControl->childSize.x() + gameListControl->sideSliderWidth, rect.h() - optionSpace);
	gameListControl->layout(saveFolderListControlRect);
}

void gameSelector::render(cveci2& position, const texture& renderTarget)
{
	renderOptionsBackGround(crectanglei2(position, rect.size), renderTarget);
	renderChildren(position, renderTarget);
}