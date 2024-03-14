#include "wanderAI.h"
#include "mob.h"
bool wanderAI::continueExecuting()
{

	return true;
}

void wanderAI::startExecuting()
{
	targetX = connectedEntity->position.x();
}

void wanderAI::updateTask()
{
	mob* connectedMob = (mob*)connectedEntity;
	if (randChance(currentRandom, 0x100) || ((abs(targetX - connectedEntity->position.x()) < 1) && randChance(currentRandom, 0x10)))
	{
		cfp targetOffset = 0x10;
		//choose new target
		targetX = connectedEntity->position.x() + randFp(currentRandom, -targetOffset, targetOffset);
	}
	connectedMob->goToPosition(cvec2(targetX, connectedEntity->position.y()));

	connectedMob->flipBodyToWalkingDirection();
	connectedMob->lookForward();
}

void wanderAI::resetTask()
{
	targetX = connectedEntity->position.x();
}

wanderAI::wanderAI(entity* connectedEntity) : taskAI(connectedEntity)
{
	resetTask();
}

void wanderAI::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"target x"), targetX);
}