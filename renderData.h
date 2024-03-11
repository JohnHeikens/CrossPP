#pragma once
#include "include/math/mattnxn.h"
#include "include/math/graphics/texture.h"
#include "GlobalFunctions.h"
#include "math/vectn.h"
struct gameControl;
struct renderData
{
	mat3x3 worldToRenderTargetTransform = mat3x3();
	mat3x3 renderTargetToWorldTransform = mat3x3();
	const texture& renderTarget;

	renderData(cmat3x3& worldToRenderTargetTransform, const texture& renderTarget) :
		worldToRenderTargetTransform(worldToRenderTargetTransform),
		renderTargetToWorldTransform(worldToRenderTargetTransform.inverse()),
		renderTarget(renderTarget) {}
	renderData clone(cmat3x3& newMatrix) const {
		return renderData(newMatrix, renderTarget);
	}
};