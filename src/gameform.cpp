#include "gameForm.h"
#include "application/cursorhandling.h"
#include "cpuUsageID.h"
#include "idConverter.h"
#include "gameForm.h"
#include "tag.h"
#include "furnaceRecipe.h"
#include "worldSelector.h"
#include "worldCreator.h"
#include "mainMenu.h"
#include "jigsawPool.h"
#include "settings.h"
#include "resourcePack.h"
#include "benchMark.h"
#include "musicManager.h"
#include "musicList.h"
#include "worldCreator.h"
#include "serverSelector.h"
#include <random>
#include <vector>
#include "constants.h"
#include "application/application.h"
#include "application/control/control.h"
#include "application/control/form.h"
#include "GlobalFunctions.h"
#include "math/graphics/texture.h"
#include "math/random/random.h"
#include "math/rectangletn.h"
#include "math/timemath.h"
#include "math/vectn.h"
#include "optimization/benchmark.h"
#include "recipe.h"
#include "soundhandler2d.h"
#include "structure.h"
#include "serverEditor.h"
#include "client.h"
#include "accountEditor.h"
#include <numeric>

application* currentApplication = nullptr;

client* currentClient = nullptr;
worldSelector* currentWorldSelector = nullptr;
serverSelector* currentServerSelector = nullptr;
serverEditor* currentServerEditor = nullptr;
gameForm* mainForm = nullptr;
worldCreator* currentWorldCreator = nullptr;
mainMenu* currentMainMenu = nullptr;
accountEditor* currentAccountEditor = nullptr;


std::vector<structure*> structureList = std::vector<structure*>();

std::vector<recipe*> craftingRecipes = std::vector<recipe*>();
std::vector<furnaceRecipe*> furnaceRecipes = std::vector<furnaceRecipe*>();


soundHandler2d* handler;

//entity* currentPlayableCharachter = nullptr;

constexpr int normalSmeltingTime = 10 * ticksPerRealLifeSecond;

world* currentWorld = nullptr;

std::mt19937 worldRandom;
std::mt19937 currentRandom;

gameForm::~gameForm()
{

	for (int i = 0; i < craftingRecipes.size(); i++)
	{
		delete craftingRecipes[i];
	}

	for (int i = 0; i < furnaceRecipes.size(); i++)
	{
		delete furnaceRecipes[i];
	}

	for (int i = 0; i < tagList.size; i++)
	{
		delete tagList[i];
	}

	for (int i = 0; i < jigsawPoolList.size(); i++)
	{
		delete jigsawPoolList[i];
	}

	for (int i = 0; i < converterList.size(); i++)
	{
		delete converterList[i];
	}

	for (int i = 0; i < structureList.size(); i++)
	{
		delete structureList[i];
	}
	unloadResourcePacks();
}

bool gameForm::close()
{
	//can't close if you're not on the main menu
	return currentMainMenu->visible;
}

void gameForm::layout(crectanglei2& rect)
{
	form::layout(rect);
	for (control* c : children) {
		c->layout(rect);
	}
	//currentMainMenu->layout(rect);
	//currentWorldSelector->layout(rect);
	//currentWorldCreator->layout(rect);
	//currentServerSelector->layout(rect);
	//currentServerEditor->layout(rect);
	//currentClient->layout(rect);
	//currentAccountEditor->layout(rect);
}

gameForm::gameForm() :form()
{

	currentRandom = getRandomFromSeed(getmicroseconds());


	handler = new soundHandler2d();

	loadResourcePacks();



	//load controls
	mainForm = this;

	currentMainMenu = new mainMenu();

	currentClient = new client();

	currentAccountEditor = new accountEditor();//initialize accounteditor after the client, as the account editor edits data which the client has to retrieve first

	currentWorldSelector = new worldSelector();
	currentWorldSelector->refresh();//can't put refresh() in base constructor because refresh() calls a virtual function
	currentWorldCreator = new worldCreator();

	currentServerSelector = new serverSelector();
	currentServerSelector->refresh();//can't put refresh() in base constructor because refresh() calls a virtual function
	currentServerEditor = new serverEditor();

	addChildren({ currentMainMenu, currentAccountEditor,
		currentWorldSelector, currentWorldCreator,
		currentServerSelector, currentServerEditor,
		currentClient });
	for (auto*& c : children) {
		c->visible = false;
	}
	currentMainMenu->visible = true;
	focusChild(currentMainMenu);
}

void gameForm::render(cveci2& position, const texture& renderTarget)
{
	//the game will update music for itself, depending on the situation
	if (focusedChild != currentClient)
	{
		updateMusic(mainMenuBackgroundMusic);
	}
	handler->update(currentClient->earPosition, hearingRange, settings::soundSettings::headScreenDistance, settings::soundSettings::volume);

	if (focusedChild == currentClient)// && (currentClient->focusedChild == nullptr)
	{
		hideCursor();
	}
	else
	{
		showCursor();
	}
	//if (currentPlayableCharachter)
	//{
		//update sounds
		//check music

	//mob* currentMob = (mob*)currentPlayableCharachter;
	//earPosition = currentMob->getHeadPosition();
	//TODO: fix
	//}
	//else
	//{
	//	handler->update(cvec2(), 0, 0, settings::soundSettings::volume);
	//}

	//draw all controls
	renderChildren(position, renderTarget);
	//don't draw self
	//form::render(position, renderTarget);
	
	
}