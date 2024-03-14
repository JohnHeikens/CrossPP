#include "slabData.h"
void slabData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"slab type"), (int&)type);
}