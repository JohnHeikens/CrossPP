#pragma once
#include "include/math/graphics/brush/brush.h"
constexpr color dragonDyingBeamStartColor = colorPalette::white;
constexpr color dragonDyingBeamEndColor = colorPalette::magenta;
template<typename brush0Type>
struct dragonBeamBrush : colorBrush
{
	const brush0Type* backgroundBrush;
	vec2 dragonScreenLocation = cvec2();
	cfp whiteDistanceSquared = 0;
	cfp transparencyDistanceSquared = 0;
	dragonBeamBrush(const brush0Type* backgroundBrush, cvec2& dragonScreenLocation, cfp& whiteDistance, cfp& transparencyDistance) :
		backgroundBrush(backgroundBrush), dragonScreenLocation(dragonScreenLocation), whiteDistanceSquared(math::squared(whiteDistance)), transparencyDistanceSquared(math::squared(transparencyDistance)) {}
	inline color getValue(cvec2& pos) const override
	{
		cfp distanceToDragonSquared = (pos - dragonScreenLocation).lengthSquared();
		return color::lerpcolor(
			(distanceToDragonSquared < whiteDistanceSquared) ?
			color::lerpcolor(dragonDyingBeamStartColor, dragonDyingBeamEndColor, (distanceToDragonSquared / whiteDistanceSquared)) :
			dragonDyingBeamEndColor
			, backgroundBrush->getValue(pos), math::minimum(distanceToDragonSquared / transparencyDistanceSquared, (fp)1));
	}
};