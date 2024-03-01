#pragma once
#include "ageData.h"

struct fireData : ageData
{
	fireData(cint& age = 0) : ageData(age) {}
	virtual void randomTick(tickableBlockContainer* containerIn, cveci2& position) override;
	virtual void serializeValue(nbtSerializer& s) override;
};