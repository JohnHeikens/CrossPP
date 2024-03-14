#include "uniformChoiceDrop.h"
inline std::vector<itemStack> uniformChoiceDrop::roll(const dropData& data, std::mt19937& randomToUse) const
{
	return options[randIndex(randomToUse, (int)options.size())]->roll(data, randomToUse);
}

uniformChoiceDrop::~uniformChoiceDrop()
{
	for (lootTable* const& option : options)
	{
		delete option;
	}
}