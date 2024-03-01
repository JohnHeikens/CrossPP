#include "endPortalFrameData.h"
void endPortalFrameData::serializeValue(nbtSerializer& s)
{
	blockData::serializeValue(s);
	facingData::serializeValue(s);
	s.serializeValue(std::wstring(L"has eye"), hasEye);
}