#pragma once
#include "renderData.h"
#include "GlobalFunctions.h"
struct gameControl;
struct gameRenderData : public renderData {
	const gameControl& screen;
	seconds secondsOffset;
	fp tickPartOffset;
	gameRenderData(cmat3x3& worldToRenderTargetTransform, const texture& renderTarget, const gameControl& screen, cfp& secondsOffset = 0) :
		renderData(worldToRenderTargetTransform, renderTarget),
		secondsOffset(secondsOffset), tickPartOffset(secondsOffset* ticksPerRealLifeSecond), screen(screen) {}

	constexpr vec2 getRenderOffset(cvec2& oldPosition, cvec2& newPosition) const
	{
		return (newPosition - oldPosition) * tickPartOffset;
	}
	constexpr vec2 getRenderPosition(cvec2& oldPosition, cvec2& newPosition) const
	{
		return oldPosition + getRenderOffset(oldPosition, newPosition);
	}
	gameRenderData clone(cmat3x3& newMatrix) const {
		return gameRenderData(newMatrix, renderTarget, screen, secondsOffset);
	}
};