#pragma once
#include "entity.h"
#include "itemStack.h"
struct floatingSlot : public entity
{
	itemStack stack;
	floatingSlot(dimension* dimensionIn, cvec2& position);
	int ticksFloating = 0;
	virtual void tick() override;
	virtual void render(const gameRenderData& targetData) const override;
	virtual ~floatingSlot() override;
	virtual void serializeValue(nbtSerializer& s) override;
};