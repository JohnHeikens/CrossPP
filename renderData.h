#pragma once
#include "include/math/mattnxn.h"
#include "include/math/graphics/texture.h"
#include "include/math/timemath.h"
#include "constants.h"
struct renderData
{
	mat3x3 worldToRenderTargetTransform = mat3x3();
	mat3x3 renderTargetToWorldTransform = mat3x3();
	const texture& renderTarget;

	renderData(cmat3x3& worldToRenderTargetTransform, const texture& renderTarget, cfp& secondsOffset = 0) :
		worldToRenderTargetTransform(worldToRenderTargetTransform),
		renderTargetToWorldTransform(worldToRenderTargetTransform.inverse()),
		renderTarget(renderTarget), secondsOffset(secondsOffset), tickPartOffset(secondsOffset * ticksPerRealLifeSecond) {}
	seconds secondsOffset;
	fp tickPartOffset;
	constexpr vec2 getRenderOffset(cvec2& oldPosition, cvec2& newPosition) const
	{
		return (newPosition - oldPosition) * tickPartOffset;
	}
	constexpr vec2 getRenderPosition(cvec2& oldPosition, cvec2& newPosition) const
	{
		return oldPosition + getRenderOffset(oldPosition, newPosition);
	}
};