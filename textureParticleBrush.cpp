#include "textureParticleBrush.h"
#include "animatedParticleData.h"
#include "particleTypeData.h"
#include "world.h"
void textureParticleBrush::render(crectangle2& rect, const renderData& targetData) const
{
	const resolutionTexture& tex = getTexture();
	fillTransparentTexture(mat3x3::cross( targetData.worldToRenderTargetTransform, mat3x3::fromRectToRect(tex.getClientRect(), rect)), tex, targetData.renderTarget);
}

const resolutionTexture& textureParticleBrush::getTexture() const
{
	const animatedParticleData* data = (const animatedParticleData*)particleTypeDataList[particleType];
	return *data->animatedTextures[currentWorld->ticksSinceStart % data->animatedTextures.size()];
}
