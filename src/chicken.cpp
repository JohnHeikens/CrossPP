#include "mobData.h"
#include "chicken.h"
#include <set>
#include <string>
#include "constants.h"
#include "entityData.h"
#include "entityID.h"
#include "experienceDrop.h"
#include "GlobalFunctions.h"
#include "math/bodypart2d.h"
#include "math/random/random.h"
#include "math/vectn.h"
#include "mob.h"
#include "nbtSerializer.h"
#include "passiveMob.h"

constexpr fp chickenAirGravityForce = 0.4;
chicken::chicken(dimension* dimensionIn, cvec2& position) :passiveMob(dimensionIn, position, entityID::chicken), mob(dimensionIn, position, entityID::chicken)
{
	timeUntilNextEgg = rand(currentRandom, minimalChickenEggTime, maximalChickenEggTime);

	//intialize body parts
	mainBodyPart = new bodyPart2D(chickenBodyTextureRect, nullptr, vec2(position.x, position.y + chickenUpperLegSize.y), chickenBodySize, chickenBodyRotationCentre, 0, 90);

	rightUpperLeg = new bodyPart2D(chickenUpperLegTextureRect, mainBodyPart, cvec2(), chickenUpperLegSize, chickenUpperLegRotationCentre);
	rightLowerLeg = new bodyPart2D(chickenLowerLegTextureRect, rightUpperLeg, cvec2(chickenUpperLegSize.x * 0.5, 0) - chickenUpperLegRotationCentre, chickenLowerLegSize, chickenLowerLegRotationCentre, 0, 90);

	leftUpperLeg = new bodyPart2D(chickenUpperLegTextureRect, mainBodyPart, cvec2(), chickenUpperLegSize, chickenUpperLegRotationCentre, 0, 90);
	leftLowerLeg = new bodyPart2D(chickenLowerLegTextureRect, leftUpperLeg, cvec2(chickenUpperLegSize.x * 0.5, 0) - chickenUpperLegRotationCentre, chickenLowerLegSize, chickenLowerLegRotationCentre, 0, 90);

	rightWing = new bodyPart2D(chickenWingTextureRect, mainBodyPart, cvec2(chickenBodySize.x * 0.5, chickenBodySize.y) - chickenBodyRotationCentre, chickenWingSize, chickenWingRotationCentre);
	leftWing = new bodyPart2D(chickenWingTextureRect, mainBodyPart, cvec2(chickenBodySize.x * 0.5, chickenBodySize.y) - chickenBodyRotationCentre, chickenWingSize, chickenWingRotationCentre);

	head = new bodyPart2D(chickenHeadTextureRect, mainBodyPart, vec2(chickenBodySize.x, chickenBodySize.y) - chickenBodyRotationCentre, chickenHeadSize, chickenHeadRotationCentre);

	bodyPart2D* wattles = new bodyPart2D(chickenWattlesTextureRect, head, cvec2(chickenHeadSize.x, chickenWattlesSize.y) - chickenHeadRotationCentre, chickenWattlesSize, chickenWattlesRotationCentre);
	bodyPart2D* beak = new bodyPart2D(chickenBeakTextureRect, head, cvec2(chickenHeadSize.x, chickenWattlesSize.y + chickenBeakSize.y * 0.5) - chickenHeadRotationCentre, chickenWattlesSize, chickenBeakRotationCentre);

	head->children.push_back(wattles);
	head->children.push_back(beak);

	rightUpperLeg->children.push_back(rightLowerLeg);
	leftUpperLeg->children.push_back(leftLowerLeg);

	mainBodyPart->children.push_back(leftWing);
	mainBodyPart->children.push_back(leftUpperLeg);
	mainBodyPart->drawOrderIndex = mainBodyPart->children.size();
	mainBodyPart->children.push_back(head);
	mainBodyPart->children.push_back(rightUpperLeg);
	mainBodyPart->children.push_back(rightWing);
}

void chicken::updateBodyParts() const
{
	mainBodyPart->translate = position + cvec2(0, chickenLowerLegSize.y + chickenUpperLegSize.y);

	cfp rightLegangle = ((mobData*)entityDataList[(int)entityType])->legSwingSynchronizer.getSwingAngle(totalLegDistance);

	rightUpperLeg->angle = rightLegangle;
	leftUpperLeg->angle = -rightLegangle;

	rightLowerLeg->angle = rightUpperLeg->angle * -0.5;
	leftLowerLeg->angle = leftUpperLeg->angle * -0.5;

	rightUpperLeg->changed = true;
	leftUpperLeg->changed = true;
	rightLowerLeg->changed = true;
	leftLowerLeg->changed = true;
	mainBodyPart->changed = true;
}

void chicken::serializeValue(nbtSerializer& s)
{
	mob::serializeValue(s);
	s.serializeValue(std::wstring(L"time until next egg"), timeUntilNextEgg);
}

fp chicken::getGravityForce() const
{
	return ((fluidArea > 0) || (!flying)) ? mob::getGravityForce() : chickenAirGravityForce;
}

bool chicken::goToPosition(cvec2& destination)
{
	flying = !onGround && (speed.y < 0);
	return mob::goToPosition(destination);
}