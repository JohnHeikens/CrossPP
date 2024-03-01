#pragma once
#include "include/interface/inamable.h"
#include <vector>
#include <memory>
#include "include/math/graphics/resolutiontexture.h"
struct animatedParticleData : INamable
{
	animatedParticleData(const std::wstring& name);
	std::vector<resolutionTexture*> animatedTextures = std::vector<resolutionTexture*>();
};