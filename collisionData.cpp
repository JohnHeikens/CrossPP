#include "collisionData.h"
#include <math/collisions.h>
void collisionData::evaluate(crectangle2& hitboxToTest, cvec2& hitboxSpeed, cfp& maxDuration)
{
	collisionTime = collideTime2d(hitboxToTest, hitboxCollidingWith, (hitboxSpeed - speed) * maxDuration, axisCollided) * maxDuration;
	if (isnan(collisionTime))
	{
		handleError(std::wstring(L"error"));
	}
}