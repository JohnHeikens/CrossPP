#pragma once
#include "math/vectn.h"
struct dimension;
struct experienceDrop
{
	int minimum = 0, maximum = 0;
	experienceDrop(int minimum = 0, int maximum = 0) :minimum(minimum), maximum(maximum) {}
	virtual void dropLoot(dimension* dimensionIn, cvec2& position) const;
};