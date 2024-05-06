#include "fireworkData.h"
#include "nbtSerializer.h"
#include "serializer/serializeList.h"
void fireWorkData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"flight duration"), flightDuration);
	serializeListOfSerializables(s, std::wstring(L"firework stars"), fireworkStars);
}