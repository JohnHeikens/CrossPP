#include "openData.h"
#include "nbtSerializer.h"
void openData::serializeValue(nbtSerializer& s)
{
	blockData::serializeValue(s);
	s.serializeValue(std::wstring(L"is open"), isOpen);
}