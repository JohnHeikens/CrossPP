#pragma once
#include "lootTable.h"
#include "amountDistribution.h"
struct rollDrop : public lootTable
{
	amountDistribution* distribution = nullptr;
	lootTable* toRoll = nullptr;
	rollDrop(lootTable* toRoll, amountDistribution* distribution = nullptr) :toRoll(toRoll), distribution(distribution) {}
	inline std::vector<itemStack> roll(const dropData& data, std::mt19937& randomToUse) const override;
	virtual ~rollDrop() override;
};