#include "dimension.h"
#include "zombie.h"
#include "taskList.h"
#include "attackOnCollideAI.h"
#include "wanderAI.h"
zombie::zombie(dimension* dimensionIn, cvec2& position, const entityID& entityType) :humanoid(dimensionIn, position, entityType)
{
	tasks = new taskList(this, {
	new attackOnCollideAI(this, entityID::human),
	new wanderAI(this),
		});


	initializeBodyParts(zombieHeadTextureRect, zombieBodyTextureRect, zombieLeftLegTextureRect, zombieRightLegTextureRect, zombieLeftArmTextureRect, zombieRightArmTextureRect, zombiePixelSize);
	itemHolding = new itemStack();
}
void zombie::serializeValue(nbtSerializer& s)
{
	humanoid::serializeValue(s);
	itemHolding->serialize(s, std::wstring(L"item holding"));
}