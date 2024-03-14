#include "choiceDrop.h"
#include "array/arrayfunctions.h"
inline std::vector<itemStack> choiceDrop::roll(const dropData& data, std::mt19937& randomToUse) const
{
	return options[randIndex(randomToUse, chances, getSum<fp>(chances))]->roll(data, randomToUse);
}

choiceDrop::~choiceDrop()
{
	for (lootTable* const& option : options)
	{
		delete option;
	}
}
