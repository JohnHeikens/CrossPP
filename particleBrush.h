#pragma once
#include "gameRenderData.h"
#include "particleID.h"
#include "nbtSerializable.h"

struct particleBrush : nbtSerializable
{
	particleID particleType = particleID();
	particleBrush(const particleID& particleType) :particleType(particleType) {}
	virtual void render(crectangle2& rect, const gameRenderData& targetData) const = 0;
};