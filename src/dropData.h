#pragma once
#include "blockData.h"
#include "constants.h"
struct dropData
{
	//0 when there was no explosion
	int explosionPower = 0;
	const itemStack* toolUsed = nullptr;
	blockData* harvestedBlockData = nullptr;
	dropData(blockData* const& harvestedBlockData = nullptr, const itemStack* toolUsed = nullptr, cint& explosionPower = 0) :toolUsed(toolUsed), harvestedBlockData(harvestedBlockData), explosionPower(explosionPower) {}
};