#include "itemParticleBrush.h"
#include "itemData.h"
#include "nbtSerializer.h"
itemParticleBrush::itemParticleBrush() : rectParticleBrush(particleID::item)
{
}

itemParticleBrush::itemParticleBrush(const itemID& brushItemID) : rectParticleBrush(particleID::item, getParticleBrushRect(*itemList[brushItemID]->tex)), brushItemID(brushItemID)
{
}

const resolutionTexture& itemParticleBrush::getTexture() const
{
	return *itemList[brushItemID]->tex;
}

void itemParticleBrush::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"brush item id"), (int&)brushItemID);
	rectParticleBrush::serializeValue(s);
}