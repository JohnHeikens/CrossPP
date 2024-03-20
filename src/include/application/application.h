#pragma once
#include "control/form.h"
#include "math/graphics/graphics.h"
#include <sfml/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "clientInput.h"
#include "math/vectn.h"
#include "GlobalFunctions.h"

struct application:IDestructable
{
	//WINDOWPLACEMENT g_wpPrev = WINDOWPLACEMENT();

	//pointer because a copy constructor isn't implemented
	sf::RenderWindow* window = nullptr;
	sf::Texture windowTexture;
	sf::Sprite windowSprite;
	clientInput input = clientInput();

	//data
	std::wstring windowCaption = std::wstring(L"");
	//colorb* windowColorPtr = nullptr;
	//graphicsObject graphics = graphicsObject();
	texture graphics = texture(cvect2<fsize_t>());
	form* mainForm;

	//vecl2 MousePos = vecl2();
	//clientInput input = clientInput();
	fp cappedFps = 60;
	//function pointer to initialize the form
	//void changeKeyboardLayout();
	int run();
	void layout(crectanglei2& newRect);
	void processInput();
	void linkGraphics();
	//static application* getApplicationConnected(HWND mainWindow);
	virtual ~application() override;
	application(form* mainForm);
	bool isFullScreen = false;
	void switchFullScreen();
};
