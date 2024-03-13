#include "mainMenu.h"
#include "textureList.h"
#include "worldSelector.h"
#include "gameForm.h"
#include "keyID.h"
#include "serverSelector.h"
#include "main.h"
#include "accountEditor.h"
#include "minecraftFont.h"
mainMenu::mainMenu() :form()
{
	borderSize = 0;
	backgroundPicture = new pictureBox(mainMenuBackgroundTexture->scaledTextures[0]);
	gameNameLabel = new label(gameName);
	gameNameLabel->backGroundColor = colorPalette::transparent;
	gameNameLabel->borderSize = 0;
	//gameNameLabel->currentFont = new baseFont(defaultTheme().font->family, 0);
	gameNameLabel->currentFont = new minecraftFont();
	addChildren({ backgroundPicture, gameNameLabel, playOfflineButton , playOnlineButton, accountButton });
}

void mainMenu::render(cveci2& position, const texture& renderTarget)
{
	form::render(position, renderTarget);
}

void mainMenu::mouseDown(cveci2& position, cvk& button)
{
	control* highestChild = getHighestChild(position);
	if (highestChild == playOfflineButton)
	{
		mainForm->switchVisibleChild(currentWorldSelector);
	}
	else if (highestChild == playOnlineButton)
	{
		mainForm->switchVisibleChild(currentServerSelector);
	}
	else if (highestChild == accountButton)
	{
		mainForm->switchVisibleChild(currentAccountEditor);
	}
}

void mainMenu::layout(crectanglei2& newRect)
{
	form::layout(newRect);
	backgroundPicture->layout(newRect);
	layoutTableCentered(std::vector<control*>({playOfflineButton, playOnlineButton, accountButton}));

	cfp stringLength = defaultTheme().font->measureStringSize(cvec2(rect.size), gameName).x() / defaultTheme().font->fontSize;
	gameNameLabel->currentFont->fontSize = rect.w() / (stringLength * 2);
	rectanglei2 gameNameRect = rectanglei2(rect.w() / 4, rect.h() - (int)(gameNameLabel->currentFont->fontSize * 2), rect.w() / 2, (int)gameNameLabel->currentFont->fontSize);
	gameNameLabel->layout(gameNameRect);
}

void mainMenu::focus()
{
	control::focus();
	focusChild(playOfflineButton);
}

void mainMenu::keyDown(cvk& key)
{
	if (key == (cvk)keyID::escape)
	{
		PostMessage(currentApplication->hwnd, WM_CLOSE, 0, 0);
	}
	form::keyDown(key);
	if (key == (cvk)VK_SPACE)//pressed space button
	{
		mainForm->switchVisibleChild(currentWorldSelector);
		return;
	}
}