#include "main.h"
// #include <crtdbg.h>
#include <iostream>
#include "constants.h"
#include "gameForm.h"
#include "application/application.h"
#include "GlobalFunctions.h"
#include "array/wstring.h"
#include <thread>
#include <filesystem>
#include <string>
#include "folderList.h"
#include <filesystem/filemanager.h>

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include "application/thread/setThreadName.h"

#if onWindows
#include "windowsIncluder.h"
#include <corecrt.h>
#include <shlobj.h>
#endif

std::filesystem::path getCommonAppDataFolder()
{

#if onWindows
	TCHAR szPath[MAX_PATH];
	// https://stackoverflow.com/questions/2899013/how-do-i-get-the-application-data-path-in-windows-using-c
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
	{
		// to make this work, #define UNICODE is defined in "windowsIncluder.h"
		return std::filesystem::path(&szPath[0]);
	}
#elif onLinux
	return L"/user/share";
#elif onMac
	return L"/Library/Application Support/";
#endif

	handleError(L"common appdata folder not found");
	return std::filesystem::path();
}

// #include <imgui-SFML.h>
// #include <imgui.h>

int main(int argc, char *argv[])
{
	// check if the application is installed or if we're debugging
	//
	if (!std::filesystem::exists(L"data"))
	{ // this application is not deployed
	  // change data folder to %appdata%/JohnHeikens/Medieval Survival/data
	  //
	  // TODO: get common appdata folder on every system
		std::filesystem::path commonAppDataFolder = getCommonAppDataFolder();
		std::filesystem::path newWorkingDirectory = commonAppDataFolder / L"JohnHeikens" / L"Medieval Survival";
		if (std::filesystem::exists(newWorkingDirectory))
		{
			std::filesystem::current_path(newWorkingDirectory); // setting path
		}
		else
		{
			handleError(newWorkingDirectory.wstring() + L" not found");
		}
		//
		// check if it worked
		if (std::filesystem::exists(L"data"))
		{
			std::cout << "true";
		}
		else
		{
			handleError(L"data folder not found in " + commonAppDataFolder.wstring());
		}
	}
	workingDirectory = std::filesystem::current_path();
	createFoldersIfNotExists(savesFolder);
	//
	// fontFamily* family = new fontFamily(new texture(std::wstring(L"data\\ascii.png"), true));
	// defaultTheme = new theme(new font(family));
	currentApplication = new application(new gameForm(), gameName);
	setCurrentThreadName(L"client thread");
	cint &result = currentApplication->run();
	//
	//
	delete currentApplication;
	//_CrtDumpMemoryLeaks();
	return result;
}

std::filesystem::path workingDirectory;
//

//
// std::wstring getUserAppDataFolder()
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
// int WINAPI WinMain(
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
//		std::wstring newWorkingDirectory = commonAppDataFolder / L"\\JohnHeikens\\Medieval Survival";
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