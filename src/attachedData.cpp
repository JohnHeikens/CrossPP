#include "attachedData.h"
#include "nbtSerializer.h"
void attachedData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"attached"), attached);
}