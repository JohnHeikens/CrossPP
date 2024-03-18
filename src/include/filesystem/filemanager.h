#pragma once
#include "GlobalFunctions.h"

//folders:

//#ifdef _HAS_CXX17 //warning! this is not a safe macro
//c++ 17:
#include <filesystem>
//#include <Windows.h>
namespace stdFileSystem = std::filesystem;

typedef int fileOpenMode;

//#else
////c++ 15:
//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace stdFileSystem = std::experimental::filesystem;
//typedef std::ios_base::open_mode fileOpenMode;
//#endif

//not cross-platform
//inline void movePathsToRecycleBin(const std::vector<std::wstring>& pathsToDelete)
//{
//	
//	std::wstring mergedString = std::wstring(L"");
//	for (const auto& it : pathsToDelete)
//	{
//		mergedString += it;
//	}
//
//	//https://devblogs.microsoft.com/oldnewthing/20100218-00/?p=14893
//	//not mergedstring += std::wstring(L"\0") because it won't add anything
//	mergedString += (letter)0;
//
//	SHFILEOPSTRUCT f;
//	ZeroMemory(&f, sizeof(SHFILEOPSTRUCT));
//	f.wFunc = FO_DELETE;
//	f.fFlags = FOF_ALLOWUNDO;
//	f.pFrom = mergedString.c_str();
//	//SHFileOperation ignores pTo in the delete operation. U must specify FOF_ALLOWUNDO to move the files to the Recycle Bin, or else.....it'll be not be place there!!!!!! How sad!!! Heh heh heh!!! :-D
//	SHFileOperation(&f);
//}
//void movePathsToRecycleBin(const std::vector<std::wstring>& pathsToDelete)
//{
//    for (const auto& path : pathsToDelete)
//    {
//        try
//        {
//            std::filesystem::remove_all(std::filesystem::path(path));
//        }
//        catch (const std::filesystem::filesystem_error& ex)
//        {
//            // Handle errors if necessary
//        }
//    }
//}

inline std::wstring findFileRecursive(const std::wstring& fileNameWithExtionsion, const std::wstring& folderIn)
{
	for (const auto& fileIterator : stdFileSystem::recursive_directory_iterator(folderIn))
	{
		const std::wstring currentFileNameWithExtension = fileIterator.path().filename().wstring();
		if (stdFileSystem::path(currentFileNameWithExtension).compare(stdFileSystem::path(fileNameWithExtionsion)) == 0)
		{
			return fileIterator.path().wstring();
		}
	}
	return std::wstring(L"");
}

//only creates last directory
inline void createFolderIfNotExists(const std::wstring& path)
{
	if (!stdFileSystem::is_directory(path) || !stdFileSystem::exists(path))
	{
		stdFileSystem::create_directory(path); // create folder
	}
}

//creates the whole path to this directory
inline void createFoldersIfNotExists(const std::wstring& path)
{
	if (!stdFileSystem::is_directory(path) || !stdFileSystem::exists(path))
	{
		stdFileSystem::create_directories(path); // create folder
	}
}
