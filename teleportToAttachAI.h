#pragma once
#include "taskAI.h"
struct teleportToAttachAI : taskAI
{
	virtual bool shouldExecute() override;
	virtual void updateTask();
	teleportToAttachAI(entity* const& connectedEntity = nullptr) : taskAI(connectedEntity) {}
};