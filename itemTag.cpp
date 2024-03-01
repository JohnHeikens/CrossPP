#include "itemTag.h"
#include "potionData.h"
#include "bucketData.h"
#include "durabilityData.h"
#include "idAnalysis.h"
#include <list>
#include "itemID.h"
itemTag* createItemTag(const itemID& id)
{
	if ((id == itemID::water_bucket) || (id == itemID::lava_bucket))
	{
		return new bucketData();
	}
	else if (id == itemID::potion)
	{
		return new potionData();
	}
	else if (hasDurability(id))
	{
		return new durabilityData();
	}
	else
	{
		return nullptr;
	}
}
void itemTag::serializeValue(nbtSerializer& s)
{
	handleError(std::wstring(L"not implemented"));
}

itemTag::~itemTag()
{
}