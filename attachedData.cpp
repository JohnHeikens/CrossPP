#include "attachedData.h"
void attachedData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"attached"), attached);
}