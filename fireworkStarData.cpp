#include "fireworkStarData.h"
#include "idConverter.h"
void fireWorkStarData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"flicker"), flicker);
	s.serializeValue(std::wstring(L"trail"), trail);
	idConverter::serializeID(s, std::wstring(L"shape"), shape, s.converter ? &s.converter->fireworkShapeIDConverter : nullptr);
	s.serializeList(std::wstring(L"colors"), colors);
	s.serializeList(std::wstring(L"fade colors"), fadeColors);
}