#include "saveFileControl.h"
#include "folderList.h"
#include "constants.h"

saveFileControl::saveFileControl(const std::wstring& gameName) : gameTile(gameName)
{
	this->gameName = gameName;
	const std::wstring currentSaveFolder = savesFolder / gameName;
	const std::wstring currentSaveFolderScreenShot = currentSaveFolder / saveFolderScreenShot;
	if (stdFileSystem::exists(currentSaveFolderScreenShot))
	{
		screenShot->backGroundImage = new texture(currentSaveFolderScreenShot, true);
	}
}