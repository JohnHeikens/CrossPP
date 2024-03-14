#pragma once
#include "fourLeggedPassiveMob.h"
//sheep
constexpr rectangle2 sheepBodyTextureRect = crectangle2(42, 2, 6, 16);
constexpr rectangle2 sheepFrontRightLegTextureRect = crectangle2(0, 0, 4, 12);
constexpr rectangle2 sheepFrontLeftLegTextureRect = crectangle2(8, 0, 4, 12);
constexpr rectangle2 sheepHeadTextureRect = crectangle2(0, 18, 8, 6);
constexpr fp sheepHeadTextureOffset = 1;
constexpr vec2 sheepHitboxSize = cvec2(0.9, 1.3);

constexpr fp sheepTextureScale = sheepHitboxSize.y() / (fp)(sheepBodyTextureRect.size.x() + sheepFrontRightLegTextureRect.size.y() + sheepHeadTextureOffset);

constexpr fp sheepHeadOffset = sheepTextureScale * sheepHeadTextureOffset;
constexpr fp sheepLegOffset = sheepTextureScale;

constexpr vec2 sheepBodySize = cvec2(sheepBodyTextureRect.size.y() * sheepTextureScale, sheepBodyTextureRect.size.x() * sheepTextureScale);
constexpr vec2 sheepBodyRotationCentre = cvec2(sheepBodySize.x() * 0.5, 0);

constexpr vec2 sheepLegSize = cvec2(sheepFrontRightLegTextureRect.size.x() * sheepTextureScale, sheepFrontRightLegTextureRect.size.y() * sheepTextureScale);
constexpr vec2 sheepLegRotationCentre = cvec2(sheepLegSize.x() * 0.5, sheepLegSize.y());

constexpr vec2 sheepFrontLegOffset = cvec2(sheepBodySize.x() * 0.5 - sheepLegSize.x() + sheepLegRotationCentre.x() - sheepLegOffset, 0);
constexpr vec2 sheepBackLegOffset = cvec2(-sheepBodySize.x() * 0.5 + sheepLegRotationCentre.x() - sheepLegOffset, 0);
constexpr vec2 sheepHeadSize = cvec2(sheepHeadTextureRect.size.x(), sheepHeadTextureRect.size.y()) * sheepTextureScale;
constexpr vec2 sheepHeadRotationCentre = cvec2(0, sheepHeadSize.y() * 0.5);
struct sheep : public fourLeggedPassiveMob
{
	colorID woolColor = (colorID)0;
	bool hasWool = false;
	sheep(dimension* dimensionIn, cvec2& position);
	virtual void serializeValue(nbtSerializer& s) override;
	virtual void render(const gameRenderData& targetData) const override;
	virtual void onDeath() override;
};