#include "facingData.h"
#include "nbtSerializer.h"
void facingData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"facing"), directionFacing);
}