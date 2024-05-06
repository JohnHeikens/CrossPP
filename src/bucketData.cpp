#include "bucketData.h"
#include "nbtSerializer.h"
void bucketData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"fluid level"), fillLevel);
}