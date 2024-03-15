#pragma once
#include "control/form.h"
#include "math/graphics/graphics.h"

struct application:IDestructable
{
	//WINDOWPLACEMENT g_wpPrev = WINDOWPLACEMENT();

	//data
	std::wstring windowCaption = std::wstring(L"");
	colorb* windowColorPtr = nullptr;
	graphicsObject graphics = graphicsObject();
	form* mainForm;

	vecl2 MousePos = vecl2();
	bool lastKeyDown[0x100];
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
