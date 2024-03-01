#include "uniformAmountDistribution.h"
int uniformAmountDistribution::getAmount(cint& amountToIncrease, const dropData& data, std::mt19937& randomToUse) const
{
	return rand(randomToUse, minimum, maximum);
}