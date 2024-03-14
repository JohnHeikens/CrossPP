#pragma once
#include "itemTag.h"
struct blockData : itemTag
{
	bool ticked = false;
	blockData() {}
	//returns if it still needs to be ticked
	virtual bool tick(tickableBlockContainer* containerIn, cveci2& position);
	virtual void randomTick(tickableBlockContainer* containerIn, cveci2& position);
	virtual void onBlockRemove(tickableBlockContainer* containerIn, cveci2& position);
	virtual void serializeValue(nbtSerializer& s) override;
};
blockData* createBlockData(const blockID& block);