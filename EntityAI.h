#pragma once
#include "nbtSerializable.h"
#include "include/interface/idestructable.h"
#include "entity.h"
struct entityAI : nbtSerializable, IDestructable
{
	entityAI(entity* const& connectedEntity) :connectedEntity(connectedEntity) {}
	entity* connectedEntity = nullptr;
	virtual void execute() = 0;
};