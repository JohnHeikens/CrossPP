#include "toolDrop.h"
#include "dropData.h"
#include "itemStack.h"
bool toolDrop::checkCondition(const dropData& data, std::mt19937& randomToUse) const
{
	if (!data.toolUsed || (data.toolUsed->count == 0))
	{
		return false;
	}
	for (int i = 0; i < requiredEnchantments.size(); i++)
	{
		if (data.toolUsed->getEnchantmentLevel(requiredEnchantments[i].identifier) < requiredEnchantments[i].power)
		{
			return false;
		}
	}

	if (requiredTool != itemID())
	{
		if (!data.toolUsed || data.toolUsed->stackItemID != requiredTool)
		{
			return false;
		}
	}
	return true;
}