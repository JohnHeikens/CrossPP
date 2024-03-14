#pragma once
#include "control/form.h"
#include "math/graphics/graphics.h"


//stored in .cpp file:
RECT GetMonitorRect();
RECT GetDesktopRect();
void GetMonitorResolution(int& horizontal, int& vertical);

HWND MakeWindow(RECT windowrect, HINSTANCE hInstance, WNDPROC proc, void* userData = nullptr, bool fullScreen = false, const std::wstring& windowCaption = std::wstring(L""));

struct application:IDestructable
{
	WINDOWPLACEMENT g_wpPrev = WINDOWPLACEMENT();

	//data
	std::wstring windowCaption = std::wstring(L"");
	colorb* windowColorPtr = nullptr;
	graphicsObject graphics = graphicsObject();
	form* mainForm;

	// Global Windows/Drawing variables
	BITMAPINFO bmi;
	HBITMAP hbmp = NULL;
	HWND hwnd = NULL;
	HDC hdcMem = NULL;
	// The window's DC
	HDC wndDC = NULL;
	HBITMAP hbmOld = NULL;

	vecl2 MousePos = vecl2();
	bool lastKeyDown[0x100];
	fp cappedFps = 60;
	//function pointer to initialize the form
	void changeKeyboardLayout();
	int run();
	void processInput();
	void render();
	void linkGraphics();
	static application* getApplicationConnected(HWND mainWindow);
	virtual ~application() override;
	application(form* mainForm, HINSTANCE hInstance);
	bool isFullScreen = false;
	void switchFullScreen();
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
