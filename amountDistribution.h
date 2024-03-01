#pragma once
#include "include/filesystem/jsonReader.h"
#include <random>
#include "constants.h"
struct amountDistribution
{
	virtual int getAmount(cint& amountToIncrease, const dropData& data, std::mt19937& randomToUse) const = 0;
};
amountDistribution* parseMinimumAndMaximum(const jsonContainer& container);