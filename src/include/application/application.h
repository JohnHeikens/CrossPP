#pragma once
#include "control/form.h"
#include "math/graphics/graphics.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "clientInput.h"
#include "math/vectn.h"
#include "globalFunctions.h"
#include "interface/inamable.h"
#include "event/eventhandler.h"
#include <future>
#include "math/graphics/doubleBuffer.h"

struct application: INamable, IDestructable
{
	//WINDOWPLACEMENT g_wpPrev = WINDOWPLACEMENT();

	//pointer because a copy constructor isn't implemented
	sf::RenderWindow* window = nullptr;
	sf::Texture* windowTexture;
	sf::Sprite* windowSprite;
	clientInput input = clientInput();

	//data
	//std::wstring windowCaption = std::wstring(L"");
	//colorb* windowColorPtr = nullptr;
	//graphicsObject graphics = graphicsObject();

	doubleBuffer buffer = doubleBuffer();
	form* mainForm;

	//vecl2 MousePos = vecl2();
	//clientInput input = clientInput();
	fp cappedFps = 60;

    mat3x3 screenToApp = mat3x3();
	//function pointer to initialize the form
	//void changeKeyboardLayout();
    static sf::RenderWindow* createWindow(const std::wstring& name);
	int run();
	void runGraphics();
	void layout(crectanglei2& newRect);
	void processInput();
	void linkGraphics();
	//static application* getApplicationConnected(HWND mainWindow);
	virtual ~application() override;
	application(form* mainForm, const std::wstring& name);
	bool isFullScreen = false;
	void switchFullScreen();
    eventHandler<sf::Event> listener = eventHandler<sf::Event>();
	std::thread* renderThread = nullptr;
    std::shared_ptr<std::promise<bool>> startRendering = std::make_shared<std::promise<bool>>();
    std::shared_ptr<std::promise<void>> finishedRendering = std::make_shared<std::promise<void>>();
	//void enableGLDebugCallback();
};
