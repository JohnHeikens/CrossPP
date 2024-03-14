#pragma once
#include "fourLeggedPassiveMob.h"
//cow
constexpr rectangle2 cowBodyTextureRect = crectangle2(40, 0, 10, 18);
constexpr rectangle2 cowFrontRightLegTextureRect = crectangle2(0, 0, 4, 12);
constexpr rectangle2 cowFrontLeftLegTextureRect = crectangle2(8, 0, 4, 12);
constexpr rectangle2 cowHeadTextureRect = crectangle2(0, 18, 6, 8);
constexpr fp cowHeadTextureOffset = 1;

constexpr vec2 cowHitboxSize = cvec2(0.9, 1.4);
constexpr fp cowTextureScale = cowHitboxSize.y() / (fp)(cowBodyTextureRect.size.x() + cowFrontRightLegTextureRect.size.y() + cowHeadTextureOffset);

constexpr fp cowHeadOffset = cowTextureScale * cowHeadTextureOffset;

constexpr vec2 cowBodySize = cvec2(cowBodyTextureRect.size.y() * cowTextureScale, cowBodyTextureRect.size.x() * cowTextureScale);
constexpr vec2 cowBodyRotationCentre = cvec2(cowBodySize.x() * 0.5, 0);

constexpr vec2 cowLegSize = cvec2(cowFrontRightLegTextureRect.size.x() * cowTextureScale, cowFrontRightLegTextureRect.size.y() * cowTextureScale);
constexpr vec2 cowLegRotationCentre = cvec2(cowLegSize.x() * 0.5, cowLegSize.y());

constexpr vec2 cowFrontLegOffset = cvec2(cowBodySize.x() * 0.5 - cowLegSize.x() + cowLegRotationCentre.x(), 0);
constexpr vec2 cowBackLegOffset = cvec2(-cowBodySize.x() * 0.5 + cowLegRotationCentre.x() - cowTextureScale * 0x2, 0);
constexpr vec2 cowHeadSize = cvec2(cowHeadTextureRect.size.x(), cowHeadTextureRect.size.y()) * cowTextureScale;
constexpr vec2 cowHeadRotationCentre = cvec2(0, cowHeadSize.y() * 0.5);

struct cow : public fourLeggedPassiveMob
{
	cow(dimension* dimensionIn, cvec2& position);
};