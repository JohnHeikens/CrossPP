#pragma once
#include "blockData.h"
struct spawnerData : blockData
{
	int spawnCoolDown = 0;
	entityID entityToSpawn = entityID::zombie;
	spawnerData(const entityID& mobToSpawn = entityID::zombie, cint& spawnCoolDown = 0) :entityToSpawn(mobToSpawn), spawnCoolDown(spawnCoolDown), blockData() {}
	virtual void randomTick(tickableBlockContainer* containerIn, cveci2& position) override;
	virtual void serializeValue(nbtSerializer& s) override;
};