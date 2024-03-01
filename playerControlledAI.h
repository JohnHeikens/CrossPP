#pragma once
#include "EntityAI.h"
struct playerControlledAI final : entityAI
{
	void execute() final;

	virtual void serializeValue(nbtSerializer& s) override;

	playerControlledAI(entity* const& connectedEntity) : entityAI(connectedEntity) {}
};