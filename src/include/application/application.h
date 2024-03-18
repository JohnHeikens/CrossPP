#pragma once
#include "control/form.h"
#include "math/graphics/graphics.h"
#include <sfml/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "clientInput.h"

struct application:IDestructable
{
	//WINDOWPLACEMENT g_wpPrev = WINDOWPLACEMENT();

	sf::RenderWindow window;
	//data
	std::wstring windowCaption = std::wstring(L"");
	//colorb* windowColorPtr = nullptr;
	graphicsObject graphics = graphicsObject();
	form* mainForm;

	//vecl2 MousePos = vecl2();
	//clientInput input = clientInput();
	fp cappedFps = 60;
	//function pointer to initialize the form
	//void changeKeyboardLayout();
	int run();
	void processInput();
	void render();
	void linkGraphics();
	//static application* getApplicationConnected(HWND mainWindow);
	virtual ~application() override;
	application(form* mainForm);
	bool isFullScreen = false;
	void switchFullScreen();
};
