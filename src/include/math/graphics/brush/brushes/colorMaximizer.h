#pragma once
#include "math/graphics/brush/brush.h"
template<typename brush0Type, typename brush1Type>
struct colorMaximizer final : public colorBrush
{
	const brush0Type& brush0 = nullptr;
	const brush1Type& brush1 = nullptr;
	colorMaximizer(const brush0Type& brush0, const brush1Type& brush1) : brush0(brush0), brush1(brush1) {}
	virtual inline color getValue(cvec2& pos) const
	{
		return color::maximizeColors(brush0.getValue(pos), brush1.getValue(pos));
	}
};