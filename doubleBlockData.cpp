#include "doubleBlockData.h"
#include "doubleBlock.h"
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