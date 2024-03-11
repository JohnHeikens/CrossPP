#pragma once
#include "textureParticleBrush.h"
struct effectParticleBrush : textureParticleBrush {
	color effectColor;
	effectParticleBrush(const color& effectColor) : effectColor(effectColor), textureParticleBrush(particleID::effect)
	{
	}
	virtual void render(crectangle2& rect, const gameRenderData& targetData) const override;
};