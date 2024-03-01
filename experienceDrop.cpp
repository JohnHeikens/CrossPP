#include "experienceDrop.h"
#include "experienceOrb.h"
#include "dimension.h"
void experienceDrop::dropLoot(dimension* dimensionIn, cvec2& position) const
{
	dropExperienceOrbs(dimensionIn, position, rand(currentRandom, minimum, maximum));
}