#pragma once
#include "renderData.h"
#include "particleID.h"
#include "nbtSerializable.h"

struct particleBrush : nbtSerializable
{
	particleID particleType = particleID();
	particleBrush(const particleID& particleType) :particleType(particleType) {}
	virtual void render(crectangle2& rect, const renderData& targetData) const = 0;
};