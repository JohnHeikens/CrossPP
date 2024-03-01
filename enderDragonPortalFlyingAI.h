#pragma once
#include "taskAI.h"
struct enderDragonPortalFlyingAI : taskAI
{
	enderDragonPortalFlyingAI(entity* const& connectedEntity) :taskAI(connectedEntity) {}
	virtual bool shouldExecute() override;
	virtual void updateTask();
};