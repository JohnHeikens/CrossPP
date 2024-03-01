#pragma once
#include "entity.h"
struct tnt : public entity
{
	tnt(dimension* dimensionIn, cvec2& position);
	int ticksUntilFuse = 0;
	virtual void tick() override;
	virtual void render(const renderData& targetData) const override;
	virtual void serializeValue(nbtSerializer& s) override;
};