#include "fireworkData.h"
void fireWorkData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"flight duration"), flightDuration);
	s.serializeListOfSerializables(std::wstring(L"firework stars"), fireworkStars);
}