#pragma once
#include "generationData.h"
struct netherGenerationData :generationData
{
	netherGenerationData() {}
	virtual void serializeValue(nbtSerializer& s) override;
	virtual ~netherGenerationData() override;
	std::vector<veci2> groundPositions = std::vector<veci2>();
};