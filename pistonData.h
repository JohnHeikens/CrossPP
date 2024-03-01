#pragma once
#include "facingData.h"
#include "doubleBlockData.h"
#include "constants.h"
#include "include/GlobalFunctions.h"
#include "include/math/direction.h"
#include "include/math/uuid.h"
#include "include/math/vectn.h"
#include "nbtSerializer.h"
//max power level: pushes in one tick
constexpr size_t pistonPushLimit = 0x1000;
constexpr fp maxPushSpeed = 1;
constexpr fp maxPushProgress = 1;

struct pistonData : virtual facingData, virtual doubleBlockData
{
	powerLevel lastPowerLevelBehind = 0;
	fp pushProgress = 0;

	uuid blocksMoving = uuid();
	bool retracting = false;

	pistonData(const directionID& directionFacing = standardUpFacingBlockDirection, cbool& isPart0 = true, const powerLevel& lastPowerLevelBehind = 0, cint& pushProgress = 0) : facingData(directionFacing), lastPowerLevelBehind(lastPowerLevelBehind), pushProgress(pushProgress), doubleBlockData(isPart0) {}
	virtual bool tick(tickableBlockContainer* containerIn, cveci2& position) override;
	virtual void serializeValue(nbtSerializer& s) override;
	virtual void onBlockRemove(tickableBlockContainer* containerIn, cveci2& position) override;
	virtual fp getPushDistancePerTick() const;
	void dropBlocksMoving(tickableBlockContainer* containerIn, cveci2& position);
	vec2 getExactPistonHeadPosition(tickableBlockContainer* containerIn, cveci2& position) const;
	fp getNextPushProgress(cfp& pushSpeed) const;
};
