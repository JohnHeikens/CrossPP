#include "serverDataList.h"
#include "folderList.h"
#include "filesystem/filemanager.h"

void serverDataList::serializeValue(nbtSerializer& s)
{
	s.serializeListOfSerializables(L"servers", servers);
}

bool serverDataList::serialize(cbool& write)
{
	createFoldersIfNotExists(serversFolder);
	stdPath serverListPath = serversFolder / std::wstring(L"serverlist" + nbtFileExtension);
	return nbtSerializable::serialize(L"server list", serverListPath, write);
}
