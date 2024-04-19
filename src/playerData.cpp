#include "playerData.h"
#include "folderList.h"
#include <filesystem/filemanager.h>
void playerData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(L"uuid", id);
	s.serializeValue(L"name", name);
}

bool playerData::serialize(cbool& write)
{
	createFoldersIfNotExists(playerDataFolder);
	const stdPath& playerDataFile = playerDataFolder / (std::wstring(L"playerdata") + nbtFileExtension);
	return nbtSerializable::serialize(L"playerdata", playerDataFile, write);
}