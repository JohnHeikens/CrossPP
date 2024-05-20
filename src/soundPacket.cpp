#include "soundPacket.h"
#include <string>
#include "nbtSerializer.h"
#include "serializer/serializeColor.h"

bool soundPacket::serialize(nbtSerializer& s)
{
	serializeNBTValue(s, L"position", position);
	s.serializeValue(L"key", key);
	s.serializeValue(L"sound index", soundIndex);
	s.serializeValue(L"volume", volume);
	return s.serializeValue(L"pitch", pitch);
}
