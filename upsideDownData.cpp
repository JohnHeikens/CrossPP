#include "upsideDownData.h"
void upsideDownData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"upside down"), upsideDown);
}