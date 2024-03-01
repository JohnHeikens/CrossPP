#pragma once
#include "blockData.h"
struct sugarCaneData :blockData
{
	sugarCaneData();
	virtual void randomTick(tickableBlockContainer* containerIn, cveci2& position) override;
};