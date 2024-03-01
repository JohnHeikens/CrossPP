#pragma once
#include "taskAI.h"
struct enderDragonCirclingAI : taskAI
{
	vec2 targetPoint = cvec2();
	enderDragonCirclingAI(entity* const& connectedEntity) :taskAI(connectedEntity) {}
	virtual void startExecuting() override;
	virtual void updateTask();
	void chooseTargetPoint();
	virtual void serializeValue(nbtSerializer& s) override;
};