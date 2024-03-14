#include "ageData.h"
void ageData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"age"), age);
}