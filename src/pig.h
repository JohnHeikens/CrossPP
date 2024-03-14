#pragma once
#include "fourLeggedPassiveMob.h"

//pig
constexpr rectangle2 pigBodyTextureRect = crectangle2(46, 0, 8, 16);
constexpr rectangle2 pigFrontRightLegTextureRect = crectangle2(0, 6, 4, 6);
constexpr rectangle2 pigFrontLeftLegTextureRect = crectangle2(8, 6, 4, 6);
constexpr rectangle2 pigHeadTextureRect = crectangle2(0, 16, 8, 8);
constexpr fp pigHeadTextureOffset = 2;

constexpr vec2 pigHitboxSize = cvec2(0.9);

constexpr fp pigTextureScale = pigHitboxSize.y() / (fp)(pigBodyTextureRect.size.x() + pigFrontRightLegTextureRect.size.y() + pigHeadTextureOffset);

constexpr fp pigHeadOffset = pigTextureScale * pigHeadTextureOffset;
constexpr fp pigLegOffset = pigTextureScale * 1;

constexpr vec2 pigBodySize = cvec2(pigBodyTextureRect.size.y() * pigTextureScale, pigBodyTextureRect.size.x() * pigTextureScale);
constexpr vec2 pigBodyRotationCentre = cvec2(pigBodySize.x() * 0.5, 0);

constexpr vec2 pigLegSize = cvec2(pigFrontRightLegTextureRect.size.x() * pigTextureScale, pigFrontRightLegTextureRect.size.y() * pigTextureScale);
constexpr vec2 pigLegRotationCentre = cvec2(pigLegSize.x() * 0.5, pigLegSize.y());

constexpr vec2 pigFrontLegOffset = cvec2(pigBodySize.x() * 0.5 - pigLegSize.x() + pigLegRotationCentre.x() - pigLegOffset, 0);
constexpr vec2 pigBackLegOffset = cvec2(-pigBodySize.x() * 0.5 + pigLegRotationCentre.x() - pigLegOffset, 0);
constexpr vec2 pigHeadSize = cvec2(pigHeadTextureRect.size.x(), pigHeadTextureRect.size.y()) * pigTextureScale;
constexpr vec2 pigHeadRotationCentre = cvec2(0, pigHeadSize.y() * 0.5);


struct pig : public fourLeggedPassiveMob
{
	pig(dimension* dimensionIn, cvec2& position);
};