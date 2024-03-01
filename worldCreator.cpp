#include "worldCreator.h"
#include "world.h"
#include "gameControl.h"
#include "gameForm.h"
#include "keyID.h"
#include "worldSelector.h"
#include "folderList.h"
#include "client.h"
#include "server.h"
worldCreator::worldCreator() : form()
{
	worldCreating = new world();
	addChildren({ worldNameLabel, worldNameTextBox, 
		willBeSavedInLabel,
		worldSeedLabel,worldSeedTextBox,
		cheatsButton,
		createButton,
		cancelButton});
}

void worldCreator::mouseDown(cveci2& position, cvk& button)
{
	control* highestChild = getHighestChild(position);
	if (highestChild == createButton)
	{
		worldCreating->name = getAvailableWorldName(worldNameTextBox->text);
		//create a new world
		if (worldSeedTextBox->text.length())
		{
			long seed = 0;

			if (convertToLong(worldSeedTextBox->text, seed))
			{
				worldCreating->seed = seed;
			}
			else
			{
				worldCreating->seed = std::hash<std::wstring>()(worldSeedTextBox->text);
			}
			worldSeedTextBox->text = L"";
		}
		else
		{
			worldCreating->seed = getmicroseconds();
		}
		worldNameTextBox->text = L"";
		//'move' the new world to the game
		currentWorld = worldCreating;
		generateNewWorld();

		createServerFromCurrentWorld();


		worldCreating = new world();


		mainForm->switchVisibleChild(currentClient);
	}
	else if (highestChild == cheatsButton)
	{
		worldCreating->allowCheats = !worldCreating->allowCheats;
	}
	else if (highestChild == cancelButton)
	{
		cancelWorldCreation();
	}
	else
	{
		form::mouseDown(position, button);
	}
}

void worldCreator::keyPress(cvk& key)
{
	if (key == (cvk)keyID::escape)
	{
		cancelWorldCreation();
	}
}

void worldCreator::layout(crectanglei2& newRect)
{
	form::layout(newRect);
	//center controls
	cint offsetFromCenter = 0x3;
	cint middleX = (rect.w() - buttonSize.x()) / 2;
	cint leftSideX = rect.w() / 2 - buttonSize.x() - offsetFromCenter;
	cint rightSideX = rect.w() / 2 + offsetFromCenter;
	cint spacing = buttonSize.y() + offsetFromCenter;

	int currentY = rect.h() / 2 + spacing * 3;
	worldNameLabel->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	worldNameTextBox->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	willBeSavedInLabel->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	worldSeedLabel->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	worldSeedTextBox->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	cheatsButton->layout(crectanglei2(middleX, currentY, buttonSize.x(), buttonSize.y()));
	currentY -= spacing;
	createButton->layout(crectanglei2(leftSideX, currentY, buttonSize.x(), buttonSize.y()));
	cancelButton->layout(crectanglei2(rightSideX, currentY, buttonSize.x(), buttonSize.y()));
}

void worldCreator::render(cveci2& position, const texture& renderTarget)
{
	cheatsButton->text = std::wstring(L"Allow Cheating: ") + (worldCreating->allowCheats ? std::wstring(L"On") : std::wstring(L"Off"));
	willBeSavedInLabel->text = std::wstring(L"Will Be Saved In: ") + getAvailableWorldName(worldNameTextBox->text);
	renderOptionsBackGround(crectanglei2(position, rect.size), renderTarget);
	renderChildren(position, renderTarget);
}

void worldCreator::cancelWorldCreation()
{
	visible = false;
	currentWorldSelector->visible = true;
	mainForm->focusChild(currentWorldSelector);
}

worldCreator::~worldCreator()
{
	delete worldCreating;
}

std::wstring getAvailableWorldName(std::wstring worldName)
{

	if (worldName.length() == 0)
	{
		worldName = std::wstring(L"World");
	}
	else
	{
		std::wstring newName = std::wstring(L"");
		bool addedUnderScore = false;
		for (int i = 0; i < worldName.length(); i++)
		{
			std::wstring allowedChars = std::wstring(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-() ");
			std::wstring allowedCharsAfterStart = std::wstring(L"01234556789");
			if ((allowedChars.find(worldName[i]) != -1) || (i && (allowedCharsAfterStart.find(worldName[i]) != -1)))
			{
				newName.push_back(worldName[i]);
				addedUnderScore = false;
			}
			else
			{
				if (!addedUnderScore)
				{
					addedUnderScore = true;
					newName.push_back(L'_');
				}
			}
		}
		worldName = newName;
	}

	if (stdFileSystem::exists(savesFolder + worldName))
	{
		int index = 1;
		while (true)
		{
			const std::wstring& currentWorldName = worldName + std::wstring(L"(") + std::to_wstring(index) + std::wstring(L")");
			if (stdFileSystem::exists(savesFolder + currentWorldName))
			{
				index++;
			}
			else
			{
				return currentWorldName;
			}
		}
	}
	else
	{
		return worldName;
	}
}
