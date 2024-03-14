#include "doubleBlockData.h"
#include "doubleBlock.h"
#include <string>
#include "block.h"
#include "blockID.h"
#include "constants.h"
#include "idAnalysis.h"
#include "GlobalFunctions.h"
#include "math/direction.h"
#include "math/vectn.h"
#include "nbtSerializer.h"
veci2 getOtherPartRelativeLocation(const blockID& id, cbool& isPart0, const directionID& directionFacing)
{
	veci2 otherRelativeLocation = ((doubleBlock*)blockList[id])->part1RelativeLocation;
	if (hasFacingData(id))
	{
		if (canFaceUp(id))
		{
			otherRelativeLocation = directionVectors2D[(int)directionFacing];
			//handleError(std::wstring(L"not implemented"));
		}
		else
		{
			if (directionFacing != standardSideFacingBlockDirection)
			{
				otherRelativeLocation.x() = -otherRelativeLocation.x();
			}
		}
	}
	if (!isPart0)
	{
		otherRelativeLocation = -otherRelativeLocation;
	}
	return otherRelativeLocation;
}
void doubleBlockData::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"is part 0"), isPart0);
}