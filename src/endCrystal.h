#pragma once
#include "entity.h"
#include "math/bodypart2d.h"
//end crystal
constexpr rectangle2 endCrystalInnerSquareTextureRect = crectangle2(96, 32, 16, 16);
constexpr rectangle2 endCrystalOuterSquareTextureRect = crectangle2(32, 32, 16, 16);
constexpr rectangle2 endCrystalBaseTextureRect = crectangle2(48, 0, 24, 8);

constexpr fp endCrystalPixelSize = 1.0 / endCrystalBaseTextureRect.size.x();
constexpr vec2 endCrystalBaseSize = cvec2(1, endCrystalBaseTextureRect.size.y() * endCrystalPixelSize);
constexpr vec2 endCrystalHitboxSize = cvec2(2);
constexpr crectangle2 endCrystalRelativeHitbox = crectangle2(endCrystalHitboxSize.x() * -0.5, 0, endCrystalHitboxSize.x(), endCrystalHitboxSize.y());
struct endCrystal : public entity
{
	endCrystal(dimension* dimensionIn, cvec2& position);
	virtual void render(const gameRenderData& targetData) const override;
	bodyPart2D* innerPart = nullptr;
	bodyPart2D* outerPart = nullptr;

	void explode();
};