#include "facingData.h"
void facingData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"facing"), (int&)directionFacing);
}