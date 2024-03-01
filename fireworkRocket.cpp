#include "fireworkRocket.h"
#include "itemData.h"
void fireworkRocket::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"ticks to exist"), ticksToExist);
	data.serialize(s, std::wstring(L"item data"));
}
void fireworkRocket::render(const renderData& targetData) const
{
	fillTransparentRectangle(blockTextureRect, calculateHitBox(), targetData.worldToRenderTargetTransform, *itemList[itemID::firework_rocket]->tex, targetData.renderTarget);
}