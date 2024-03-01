#pragma once
#include "throwable.h"
struct fireBall : public throwable
{
	fireBall(dimension* dimensionIn, cvec2& position, const entityID& entityType);
	virtual void onCollision(const std::vector<entity*>& collidingEntities) override;
	virtual fp getGravityForce() const override;
	virtual std::vector<vec3> getFrictions() const override;
};