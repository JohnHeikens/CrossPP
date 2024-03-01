#include "bucketData.h"
void bucketData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"fluid level"), fillLevel);
}