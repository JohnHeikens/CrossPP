#pragma once
#include "particleBrush.h"
struct textureParticleBrush : particleBrush
{
	size_t brushIndex = 0;
	virtual void render(crectangle2& rect, const renderData& targetData) const override;
};
