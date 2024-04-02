#include "soundPacket.h"
#include <string>

bool soundPacket::serialize(nbtSerializer& s)
{
	s.serializeValue(L"position", position);
	s.serializeValue(L"sound id", soundCollectionID);
	s.serializeValue(L"sound index", soundIndex);
	s.serializeValue(L"volume", volume);
	return s.serializeValue(L"pitch", pitch);
}
