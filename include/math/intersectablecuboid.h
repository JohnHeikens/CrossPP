#pragma once
#include "intersectable.h"
#include "rectangletn.h"
struct intersectableCuboid : iIntersectable
{
	rectangle3 box;
	intersectableCuboid(crectangle3& box) :box(box) {}
	virtual bool intersect(ray& r, intersection*& result) override;
};