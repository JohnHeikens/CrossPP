#include "main.h"
#include <Windows.h>
#include <crtdbg.h>
#include <iostream>
#include "constants.h"
#include "gameForm.h"
#include "include/application/application.h"
#include "include/GlobalFunctions.h"
#include "include/array/wstring.h"
#include <thread>
#include <corecrt.h>
#include <filesystem>
#include <string>
#include <shlobj.h>
#include "folderList.h"
#include <filesystem/filemanager.h>

#include <iostream>

std::filesystem::path workingDirectory;

std::wstring getCommonAppDataFolder() {
	TCHAR szPath[MAX_PATH];
	//https://stackoverflow.com/questions/2899013/how-do-i-get-the-application-data-path-in-windows-using-c
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
	{
		return std::wstring(&szPath[0]);
	}

	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"common appdata folder not found",
		(LPCWSTR)L"Medieval Survival",
		MB_ICONWARNING | MB_OK
	);
	return std::wstring();
}

std::wstring getUserAppDataFolder()
{
	char* pValue;
	size_t len;
	errno_t err = _dupenv_s(&pValue, &len, "APPDATA");
	if (pValue && !err)
	{
		std::wstring appDataFolder = stringToWString(std::string(pValue, len));
		delete pValue;
		return appDataFolder;
	}
	int msgboxID = MessageBox(
		NULL,
		(LPCWSTR)L"user appdata folder not found",
		(LPCWSTR)L"Medieval Survival",
		MB_ICONWARNING | MB_OK
	);
	return L"";
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{

	std::wstring arguments = stringToWString(std::string(lpCmdLine));

	//check if the application is installed or if we're debugging

	if (!std::filesystem::exists(L"data")) {//this application is not deployed
		//change data folder to %appdata%/JohnHeikens/Medieval Survival/data

		std::wstring commonAppDataFolder = getCommonAppDataFolder();
		std::wstring newWorkingDirectory = commonAppDataFolder + L"\\JohnHeikens\\Medieval Survival";
		if (std::filesystem::exists(newWorkingDirectory)) {
			std::filesystem::current_path(newWorkingDirectory); //setting path
		}
		else {
			handleError(newWorkingDirectory + L" not found");
		}

		//check if it worked
		if (std::filesystem::exists(L"data")) {
			std::cout << "true";
		}
		else {
			handleError(L"data folder not found in " + commonAppDataFolder);
		}
	}
	workingDirectory = std::filesystem::current_path();
	createFoldersIfNotExists(savesFolder);

	//fontFamily* family = new fontFamily(new texture(std::wstring(L"data\\ascii.png"), true));
	//defaultTheme = new theme(new font(family)); 
	currentApplication = new application(new gameForm(), hInstance);
	currentApplication->windowCaption = gameName;
	cint& result = currentApplication->run();


	delete currentApplication;
	_CrtDumpMemoryLeaks();
	return result;
}