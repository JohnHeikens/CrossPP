#include "upsideDownData.h"
#include "nbtSerializer.h"
void upsideDownData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"upside down"), upsideDown);
}