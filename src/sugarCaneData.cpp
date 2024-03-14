#include "sugarCaneData.h"
#include "tickableBlockContainer.h"
sugarCaneData::sugarCaneData()
{

}

void sugarCaneData::randomTick(tickableBlockContainer* containerIn, cveci2& position)
{
	cint maxSugarCaneHeight = 4;
	if (containerIn->getBlockID(position + cveci2(0, -1)) != blockID::sand)
	{
		return;
	}
	int height;
	for (height = 1; height < maxSugarCaneHeight; height++)
	{
		if (containerIn->getBlockID(position + cveci2(0, height)) != blockID::sugar_cane)
		{
			break;
		}
	}
	if (height != maxSugarCaneHeight)
	{
		if (randChance(currentRandom, 0x4))
		{
			containerIn->setBlockID(position + cveci2(0, height), blockID::sugar_cane);
		}
	}
}