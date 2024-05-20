#pragma once
#include "attachedBlockData.h"
struct sugarCaneData :attachedBlockData
{
	sugarCaneData();
	virtual void randomTick(tickableBlockContainer* containerIn, cveci2& position) override;
};