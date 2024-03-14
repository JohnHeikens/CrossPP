#pragma once
#include "fireBallAI.h"
constexpr fp enderDragonSightRange = 0x40;
struct enderDragonStrafingAI : fireBallAI
{
	enderDragonStrafingAI(entity* const& connectedEntity) :fireBallAI(connectedEntity, entityID::dragon_fireball, enderDragonSightRange) {}
	virtual bool shouldExecute() override;
	virtual bool continueExecuting() override;
	virtual void updateTask();
};