#include "durabilityData.h"
void durabilityData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"durability"), durability);
}