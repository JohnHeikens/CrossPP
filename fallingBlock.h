#pragma once
#include "entity.h"
struct fallingBlock : public entity
{
	blockID linkedBlockID;
	fallingBlock(dimension* dimensionIn, cvec2& position, const blockID& linkedBlockID = blockID::sand);
	virtual void render(const renderData& targetData) const override;
	virtual void tick() override;
	virtual void serializeValue(nbtSerializer& s) override;
};