#include "application.h"
#include "math/timemath.h"
#include "interaction.h"
#include "math/graphics/graphics.h"
#include "optimization/stableTickLoop.h"
#include "control/control.h"
#include "math/graphics/texture.h"
#include <future>

#include <SFML/OpenGL.hpp>
//#include <gl/glew.h>

// works on windows only
// void application::changeKeyboardLayout()
//{
//	// Define the US English keyboard layout identifier
//	LPCWSTR US_ENGLISH_KEYBOARD_LAYOUT_ID = L"00000409";
//
//	// Load the US English keyboard layout
//	HKL usEnglishLayout = LoadKeyboardLayout(US_ENGLISH_KEYBOARD_LAYOUT_ID, KLF_ACTIVATE);
//
//	// Activate the US English keyboard layout
//	ActivateKeyboardLayout(usEnglishLayout, KLF_ACTIVATE);
//
//	// Example usage: Get the currently active keyboard layout
//	HKL activeLayout = GetKeyboardLayout(0);
//	std::wcout << L"Active Keyboard Layout: " << activeLayout << std::endl;
// }

constexpr int pixelMultiplier = onAndroid ? 4 : 1;

int application::run() {
    if (!window) {
        window = createWindow(name);
    }

    //initialize them AFTER the render window has been created, so they have a context
    windowTexture = new sf::Texture();
    windowSprite = new sf::Sprite();
    //settings = window->getSettings();
    //std::cout << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;
    //std::cout << settings.majorVersion << "." << settings.minorVersion << std::endl;

    //window->setFramerateLimit(60);
    // ImGui::SFML::Init(window);

    // Create a texture with initial size


    // Create a sprite to display the texture


    // cap at 60fps
    cmicroseconds &frameTime = (microseconds) (1000000 / cappedFps);
    stableLoop loop = stableLoop(frameTime);
    // changeKeyboardLayout();
    //windowSprite->scale(1, -1);
    //windowSprite->move(0, (float)size.y);
    //windowSprite.scale(1, -1);
    //windowSprite.move(0, (float)size.y);
    layout(crectanglei2(cveci2(), veci2(window->getSize().x, window->getSize().y)));
    mainForm->focus();
    std::future<void> updateAsync = std::future<void>();
    while (window->isOpen()) {
        loop.waitTillNextLoopTime();
        //copy the texture to a temporary texture
        const texture tCopy = graphics;

        auto asyncUpdate = [this, &tCopy]() {
            windowTexture->update((byte *) tCopy.baseArray);
            window->clear();
            //window->setActive(true);
            window->draw(*windowSprite);
            // ImGui::SFML::Render(window);
            window->display();
            //window->setActive(false);
        };
        // Start the asynchronous task
        updateAsync = std::async(std::launch::async, asyncUpdate);

        // Do stuff with graphics->colors
        mainForm->render(cveci2(0, 0), graphics);
        // Draw graphics->colors to window

        graphics.switchChannels(graphics.baseArray, 0, 2);

        if (updateAsync.valid()) {
            updateAsync.get();
        }
        sf::Keyboard::setVirtualKeyboardVisible(mainForm->wantsTextInput());

        processInput(); // process events from user right before the check if window->isOpen()
    }
    if (updateAsync.valid()) {
        updateAsync.get();
    }
    delete windowSprite;
    delete windowTexture;
    delete window;
    return 0;
}

void application::layout(crectanglei2 &newRect) {
    sf::FloatRect visibleArea(newRect.x, newRect.y, newRect.w, newRect.h);
    window->setView(sf::View(visibleArea));

    screenToApp = mat3x3::combine({
                                          mat3x3::scale(vec2(1.0 / pixelMultiplier)),
                                          mat3x3::translate(cvec2(0, -newRect.size.y)),
                                          mat3x3::mirror(axisID::y, 0)
                                  });

    window->setView(sf::View(sf::FloatRect((float) newRect.x, (float) newRect.y, (float) newRect.w,
                                           (float) newRect.h)));
    windowTexture->create(newRect.size.x, newRect.size.y);
    windowSprite->setTexture(*windowTexture, true);
    windowSprite->setScale(1, -1);
    windowSprite->setPosition(0, (float) newRect.size.y);

    graphics = texture(cvect2<fsize_t>(newRect.size));
    mainForm->layout(crectanglei2(cveci2(), newRect.size));
}

void application::processInput() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            std::cout << "got close signal";
            if (mainForm->close()) {
                window->close();
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (addWithoutDupes(input.keysHolding, event.key.code)) {
                listener.invoke(event);
            }
        } else if (event.type == sf::Event::KeyReleased) {
            const auto &it = std::find(input.keysHolding.begin(), input.keysHolding.end(),
                                       event.key.code);
            if (it != input.keysHolding.end()) {
                listener.invoke(event);
                input.keysHolding.erase(it);
            }
        } else if (is_in(event.type, sf::Event::TouchBegan, sf::Event::TouchMoved,
                         sf::Event::TouchEnded)) {
            cveci2 &correctedPos = veci2(
                    screenToApp.multPointMatrix(cvec2(event.touch.x, event.touch.y)));
            sf::Event correctedEvent = event;
            correctedEvent.touch.x = correctedPos.x;
            correctedEvent.touch.y = correctedPos.y;
            listener.invoke(correctedEvent);
        } else if (is_in(event.type, sf::Event::MouseButtonPressed,
                         sf::Event::MouseButtonReleased)) {
            cveci2 &correctedPos = veci2(
                    screenToApp.multPointMatrix(cvec2(event.mouseButton.x, event.mouseButton.y)));
            sf::Event correctedEvent = event;
            correctedEvent.mouseButton.x = correctedPos.x;
            correctedEvent.mouseButton.y = correctedPos.y;
            listener.invoke(correctedEvent);
        } else if (event.type == sf::Event::MouseMoved) {
            cveci2 &correctedPos = veci2(
                    screenToApp.multPointMatrix(cvec2(event.mouseMove.x, event.mouseMove.y)));
            sf::Event correctedEvent = event;
            correctedEvent.mouseMove.x = correctedPos.x;
            correctedEvent.mouseMove.y = correctedPos.y;
            listener.invoke(correctedEvent);
        } else if (event.type == sf::Event::Resized) {
            layout(rectanglei2(cveci2(), cveci2(event.size.width / pixelMultiplier,
                                                event.size.height / pixelMultiplier)));
        } else {
            //all other events are passed through directly
            listener.invoke(event);
        }
    }
}

// LRESULT CALLBACK WndProc(
//	HWND hwnd,
//	UINT msg,
//	WPARAM wParam,
//	LPARAM lParam)
//{
//
//	application* app = application::getApplicationConnected(hwnd);
//	switch (msg) {
//	case WM_CREATE:
//	{
//		//https://social.msdn.microsoft.com/Forums/vstudio/en-US/b9ec34b5-827b-4357-8344-939baf284f46/win32-about-createwindowex-and-wndproc-pointer-relations?forum=vclanguage
//		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
//		app = (application*)lpcs->lpCreateParams;
//
//		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);//connect the window to this application
//		app->hwnd = hwnd;//link it to the hwnd already, so the correct window is referenced to
//		app->linkGraphics();
//	}
//	break;
//
//	case WM_PASTE:
//	{
//		app->mainForm->paste();
//	}
//	break;
//
//	case WM_MOUSEMOVE:
//	{
//
//	}
//	break;
//	case WM_MOUSEWHEEL:
//	{
//		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
//		app->mainForm->scroll(app->MousePos, delta / WHEEL_DELTA);
//	}
//	break;
//	case WM_KEYDOWN:
//	{
//		if (!app->lastKeyDown[wParam])
//		{
//			app->mainForm->keyDown((vk)wParam);
//		}
//		//if (pressed(VK_CONTROL))
//		//{
//		//	if (wParam == 'V')
//		//	{
//		//		app->mainForm->paste();
//		//	}
//		//}
//		//else
//		//{
//		//
//		//}
//		app->lastKeyDown[wParam] = true;
//	}
//	break;
//	case WM_KEYUP:
//	{
//		app->mainForm->keyUp((vk)wParam);
//		app->lastKeyDown[wParam] = false;
//	}
//	break;
//	case WM_LBUTTONDOWN:
//	case WM_RBUTTONDOWN:
//	case WM_MBUTTONDOWN:
//	{
//		cvk mouseButtonPressed = msg == WM_LBUTTONDOWN ? VK_LBUTTON : msg == WM_RBUTTONDOWN? VK_RBUTTON : VK_MBUTTON;
//		if (!app->lastKeyDown[mouseButtonPressed])
//		{
//			app->mainForm->mouseDown(app->MousePos, mouseButtonPressed);
//			app->lastKeyDown[mouseButtonPressed] = true;
//		}
//	}
//	break;
//	case WM_LBUTTONUP:
//	case WM_RBUTTONUP:
//	case WM_MBUTTONUP:
//	{
//		cvk mouseButtonReleased = msg == WM_LBUTTONUP ? VK_LBUTTON : msg == WM_RBUTTONUP ? VK_RBUTTON : VK_MBUTTON;
//		//f->onMouseUp(cveci2(MousePos.x, MousePos.y));
//		app->lastKeyDown[mouseButtonReleased] = false;
//
//		app->mainForm->mouseUp(cveci2(app->MousePos.x, app->MousePos.y), mouseButtonReleased);
//	}
//	break;
//	case WM_SETFOCUS:
//	{
//		if (app->mainForm)
//		{
//			app->mainForm->focus();
//		}
//	}
//	break;
//	case WM_KILLFOCUS:
//	{
//		std::fill(app->lastKeyDown, app->lastKeyDown + 0x100, false);
//		app->mainForm->lostFocus();
//	}
//	break;
//	case WM_MDIMAXIMIZE:
//	{
//	}
//	break;
//	case WM_SIZE:
//	case WM_SIZING:
//	{
//		app->linkGraphics();
//	}
//	break;
//	case WM_SYSCOMMAND:
//	{
//		if (wParam == SC_MAXIMIZE)
//		{
//			app->switchFullScreen();
//		}
//	}
//	break;
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//		// Draw graphics->colors to window when window needs repainting
//		BitBlt(hdc, 0, 0, (int)app->graphics.size.x, (int)app->graphics.size.y, app->hdcMem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//	}
//	break;
//	case WM_CLOSE:
//	{
//		if (app->mainForm->close())
//		{
//			DestroyWindow(hwnd);
//		}
//		else
//		{
//			//cancelled
//			return 0;
//		}
//	}
//	break;
//	case WM_DESTROY:
//	{
//		SelectObject(app->hdcMem, app->hbmOld);
//		DeleteDC(app->wndDC);
//		PostQuitMessage(0);
//	}
//	break;
//	default:
//	{
//	}
//	break;
//	}
//	return DefWindowProc(hwnd, msg, wParam, lParam);
// }
//
// void application::processInput()
//{
//	POINT p;
//	if (GetCursorPos(&p))
//	{
//		//cursor position now in p.x and p.y
//		if (ScreenToClient(hwnd, &p))
//		{
//			//p.x and p.y are now relative to hwnd's client area
//			MousePos = vecl2(p.x, p.y);
//			//swap y
//			MousePos.y = (int)graphics.size.y - MousePos.y - 1;
//		}
//	}
//	if (mainForm->focused)
//	{
//		mainForm->hover(cveci2(MousePos.x, MousePos.y));
//		for (vk keyCode = 0x0; keyCode < 0xff; keyCode++)
//		{
//			if (pressed(keyCode))
//			{
//				mainForm->keyPress(keyCode);
//			}
//		}
//	}
// }

// void application::linkGraphics()
//{
//	//adjust graphics size
//	RECT graphicsrect, windowrect;
//	GetClientRect(hwnd, &graphicsrect);
//	GetWindowRect(hwnd, &windowrect);
//
//	if (graphics.baseArray)
//	{
//		//else the colors will automatically be deleted
//		if (!color::isByteColor)
//		{
//			delete[] graphics.baseArray;
//			graphics.baseArray = nullptr;
//		}
//	}
//
//	graphics = graphicsObject(cveci2(graphicsrect.right - graphicsrect.left, graphicsrect.bottom - graphicsrect.top), nullptr);
//
//	/* Use CreateDIBSection to make a HBITMAP which can be quickly
//	 * blitted to a surface while giving 100% fast access to graphics->colors
//	 * before blit.
//	 */
//	 // Desired bitmap properties
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);//sizeof(BITMAPINFO);
//	bmi.bmiHeader.biWidth = (LONG)graphics.size.x;
//	bmi.bmiHeader.biHeight = (LONG)graphics.size.y; //-graphics->size.y to order graphics->colors from top to bottom
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
//	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biSizeImage = 0;// graphics->width* graphics->size.y * 4;
//	bmi.bmiHeader.biXPelsPerMeter = 0;
//	bmi.bmiHeader.biYPelsPerMeter = 0;
//	bmi.bmiHeader.biClrUsed = 0;
//	bmi.bmiHeader.biClrImportant = 0;
//	bmi.bmiColors[0].rgbBlue = 0;
//	bmi.bmiColors[0].rgbGreen = 0;
//	bmi.bmiColors[0].rgbRed = 0;
//	bmi.bmiColors[0].rgbReserved = 0;
//	HDC hdc = GetDC(hwnd);
//	// Create DIB section to always give direct access to colors
//	hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&windowColorPtr, NULL, 0);
//	if constexpr (color::isByteColor)
//	{
//		graphics.baseArray = (color*)windowColorPtr;
//	}
//	else
//	{
//		graphics.baseArray = new color[graphics.size.x * graphics.size.y];
//	}
//
//	DeleteDC(hdc);
//	ShowWindow(hwnd, SW_SHOW);
//	// Retrieve the window's DC
//	wndDC = GetDC(hwnd);
//	// Create DC with shared colors to variable 'graphics->colors'
//	hdcMem = CreateCompatibleDC(wndDC);//HDC must be wndDC!! :)
//	hbmOld = (HBITMAP)SelectObject(hdcMem, hbmp);
//
//	mainForm->layout(crectanglei2(0, 0, (int)graphics.size.x, (int)graphics.size.y));
// }
application::application(form *mainForm, const std::wstring &name) : INamable(name) {
    graphics = texture(cvecs2());
    this->mainForm = mainForm;
    this->mainForm->addEventHandlers(&control::processEvent, listener);
    //std::fill(lastKeyDown, lastKeyDown + 0x100, false);
}

// OpenGL debug callback function

//void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
//    GLsizei length, const GLchar* message, const void* userParam)
//{
//    // Trigger breakpoint when an error occurs
//    if (type == GL_DEBUG_TYPE_ERROR)
//    {
//
//    }
//
//    // Print error message
//    std::cerr << "OpenGL Error: " << message << std::endl;
//}
//
//void application::enableGLDebugCallback()
//{
//	// Enable OpenGL debug output
//    glEnable(GL_DEBUG_OUTPUT);
//    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//    glDebugMessageCallback(debugCallback, nullptr);
//}
// void application::switchFullScreen()
//{
//	//all styles will be kept except all overlappedwindow styles
//	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
//	if (dwStyle & WS_OVERLAPPEDWINDOW)
//	{
//		//overlapped window: full screen is off
//		//set it on
//		MONITORINFO mi = { sizeof(mi) };
//		if (GetWindowPlacement(hwnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
//		{
//			SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
//			SetWindowPos(hwnd, HWND_TOP,
//				mi.rcMonitor.left, mi.rcMonitor.top,
//				mi.rcMonitor.right - mi.rcMonitor.left,
//				mi.rcMonitor.bottom - mi.rcMonitor.top,
//				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
//		}
//	}
//	else
//	{
//		//turn full screen off
//		SetWindowLong(hwnd, GWL_STYLE,
//			dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(hwnd, &g_wpPrev);
//		SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
//			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
//			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
//	}
//	isFullScreen = !isFullScreen;
//	linkGraphics();
// }
// application* application::getApplicationConnected(HWND mainWindow)
//{
//	//getwindowlong is not going to cut it. long_ptr it is!
//	LONG_PTR l = GetWindowLongPtr(mainWindow, GWLP_USERDATA);
//	return (application*)l;
// }
application::~application() {
    delete mainForm;

    // we dont have to delete its colors because they are part of the DIBSection

    //if (graphics.baseArray)
    //{
    //	graphics.baseArray = nullptr;
    //}
}

sf::RenderWindow *application::createWindow(const std::wstring& name) {
    sf::ContextSettings settings;

    settings.antialiasingLevel = 0;
    auto screenSize = sf::VideoMode::getDesktopMode();


    veci2 size = veci2(screenSize.width / pixelMultiplier, screenSize.height / pixelMultiplier);
    return new sf::RenderWindow(sf::VideoMode(size.x, size.y), WStringToString(name),
                                  sf::Style::Close | sf::Style::Resize, settings);
}
