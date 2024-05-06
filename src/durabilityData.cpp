#include "durabilityData.h"
#include "nbtSerializer.h"
void durabilityData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"durability"), durability);
}