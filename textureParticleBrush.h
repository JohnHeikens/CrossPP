#pragma once
#include "particleBrush.h"
#include "math/graphics/resolutiontexture.h"
#include "math/rectangletn.h"
#include "particleID.h"
#include "renderData.h"
struct textureParticleBrush : particleBrush
{
	size_t brushIndex = 0;
	virtual void render(crectangle2& rect, const renderData& targetData) const override;
	const resolutionTexture& getTexture() const;
	textureParticleBrush(const particleID& particleType) : particleBrush(particleType){}
};
