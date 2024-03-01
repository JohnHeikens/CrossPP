#pragma once
#include "hollowRidableEntity.h"
#include "include/math/bodypart2d.h"
struct boat :public hollowRidableEntity
{
	bodyPart2D* leftPaddleHandle = nullptr;
	bodyPart2D* rightPaddleHandle = nullptr;
	fp paddleRotations = 0;
	woodTypeID boatType = (woodTypeID)0;
	static fp getPaddleAngle(cfp& paddleRotations);
	boat(dimension* dimensionIn, cvec2& position);
	virtual void render(const renderData& targetData) const override;
	virtual void tick() override;
	virtual void onDeath() override;
	virtual void serializeValue(nbtSerializer& s) override;
	virtual vec2 getRelativeSeatPosition(cint& seatIndex) const override;
	virtual fp getHollowVolume() const final;
	virtual fp getWeightPerCubicMeter() const final;
};