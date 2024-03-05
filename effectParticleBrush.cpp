#include "stdafx.h"
#include "effectParticleBrush.h"
#include "math/graphics/brush/brushes.h"
#include "math/graphics/resolutiontexture.h"
#include "math/mattnxn.h"
#include "math/rectangletn.h"
#include "renderData.h"

void effectParticleBrush::render(crectangle2& rect, const renderData& targetData) const
{
	const resolutionTexture& tex = getTexture();
	fillTransparentRectangle(tex.getClientRect(), mat3x3::cross(targetData.worldToRenderTargetTransform, mat3x3::fromRectToRect(tex.getClientRect(), rect)),
		colorMultiplier<solidColorBrush, resolutionTexture>(solidColorBrush(effectColor), tex), targetData.renderTarget);
}
