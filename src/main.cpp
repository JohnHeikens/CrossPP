#include "main.h"
//#include <crtdbg.h>
#include <iostream>
#include "constants.h"
#include "gameForm.h"
#include "application/application.h"
#include "GlobalFunctions.h"
#include "array/wstring.h"
#include <thread>
//#include <corecrt.h>
#include <filesystem>
#include <string>
//#include <shlobj.h>
#include "folderList.h"
#include <filesystem/filemanager.h>

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>

#include <imgui-SFML.h>
#include <imgui.h>

int main(int argc, char *argv[])
{
    std::cout << "Hello World" << std::endl;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tutorial", sf::Style::Close, settings);
    settings = window.getSettings();
    std::cout << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;
    std::cout << settings.majorVersion << "." << settings.minorVersion << std::endl;

    window.setFramerateLimit(60);
    //ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            //ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        //ImGui::SFML::Update(window, deltaClock.restart());
//
        //ImGui::ShowDemoWindow();

        window.clear();
        //ImGui::SFML::Render(window);
        window.display();
    }
    //ImGui::SFML::Shutdown();

    return 0;
}

//std::filesystem::path workingDirectory;
//
//std::wstring getCommonAppDataFolder() {
//	TCHAR szPath[MAX_PATH];
//	//https://stackoverflow.com/questions/2899013/how-do-i-get-the-application-data-path-in-windows-using-c
//	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
//	{
//		return std::wstring(&szPath[0]);
//	}
//
//	handleError(L"common appdata folder not found");
//	return std::wstring();
//}
//
//std::wstring getUserAppDataFolder()
//{
//	char* pValue;
//	size_t len;
//	errno_t err = _dupenv_s(&pValue, &len, "APPDATA");
//	if (pValue && !err)
//	{
//		std::wstring appDataFolder = stringToWString(std::string(pValue, len));
//		delete pValue;
//		return appDataFolder;
//	}
//	handleError(L"user appdata folder not found");
//	return L"";
//}
//
//int WINAPI WinMain(
//	HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine,
//	int nShowCmd)
//{
//
//	std::wstring arguments = stringToWString(std::string(lpCmdLine));
//
//	//check if the application is installed or if we're debugging
//
//	if (!std::filesystem::exists(L"data")) {//this application is not deployed
//		//change data folder to %appdata%/JohnHeikens/Medieval Survival/data
//
//		std::wstring commonAppDataFolder = getCommonAppDataFolder();
//		std::wstring newWorkingDirectory = commonAppDataFolder + L"\\JohnHeikens\\Medieval Survival";
//		if (std::filesystem::exists(newWorkingDirectory)) {
//			std::filesystem::current_path(newWorkingDirectory); //setting path
//		}
//		else {
//			handleError(newWorkingDirectory + L" not found");
//		}
//
//		//check if it worked
//		if (std::filesystem::exists(L"data")) {
//			std::cout << "true";
//		}
//		else {
//			handleError(L"data folder not found in " + commonAppDataFolder);
//		}
//	}
//	workingDirectory = std::filesystem::current_path();
//	createFoldersIfNotExists(savesFolder);
//
//	//fontFamily* family = new fontFamily(new texture(std::wstring(L"data\\ascii.png"), true));
//	//defaultTheme = new theme(new font(family)); 
//	currentApplication = new application(new gameForm(), hInstance);
//	currentApplication->windowCaption = gameName;
//	cint& result = currentApplication->run();
//
//
//	delete currentApplication;
//	_CrtDumpMemoryLeaks();
//	return result;
//}