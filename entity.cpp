#pragma once
#include "entity.h"
#include "include/math/collisions.h"
#include "entityData.h"
#include "world.h"
#include "dimension.h"
#include "include/math/vectn.h"
#include "soundList.h"
#include "human.h"
#include "idAnalysis.h"
#include "lightLevel.h"
#include "pressurePlateData.h"
#include "fluidList.h"
#include "block.h"
#include "include/math/vectorrandom.h"
#include "playerControlledAI.h"
#include "fluidData.h"
#include "zombifiedPiglin.h"
#include "enderDragon.h"
#include "zombie.h"
#include "skeleton.h"
#include "fluidList.h"
#include "entityData.h"
#include "idList.h"
#include "magmaCube.h"
#include "blaze.h"
#include "ghast.h"
#include "experienceOrb.h"
#include "sheep.h"
#include "creeper.h"
#include "pig.h"
#include "cow.h"
#include "wolf.h"
#include "cod.h"
#include "chicken.h"
#include "shulker.h"
#include "enderman.h"
#include "smallFireBall.h"
#include "ghastFireBall.h"
#include "dragonFireBall.h"
#include "boat.h"
#include "floatingSlot.h"
#include "egg.h"
#include "snowBall.h"
#include "enderPearl.h"
#include "eyeOfEnder.h"
#include "fishingHook.h"
#include "arrow.h"
#include "tnt.h"
#include "endCrystal.h"
#include "fallingBlock.h"
#include "minecart.h"
#include "particle.h"
#include "piston.h"
#include "fireworkRocket.h"
#include "pollen.h"
#include "chunk.h"
#include "gameControl.h"
#include <vector>
#include <math.h>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include "armorType.h"
#include "blockID.h"
#include "chunkLoadLevel.h"
#include "collisionData.h"
#include "collisionDataCollection.h"
#include "collisionEdgeData.h"
#include "collisionTypeID.h"
#include "constants.h"
#include "damageSource.h"
#include "damageType.h"
#include "dimensionID.h"
#include "entityID.h"
#include "humanoid.h"
#include "include/array/arraynd.h"
#include "include/GlobalFunctions.h"
#include "include/math/direction.h"
#include "include/math/graphics/brush/brushes.h"
#include "include/math/graphics/color/color.h"
#include "include/math/mathfunctions.h"
#include "include/math/rectangletn.h"
#include "nbtData.h"
#include "nbtDataTag.h"
#include "nbtSerializer.h"
#include "renderData.h"
#include "slime.h"
#include "statusEffect.h"
#include "statusEffectID.h"
#include "tickableBlockContainer.h"

constexpr veci2 endBlockSpawningOn = cveci2(mainEndIslandMaxRadius / 2, 0);
constexpr vec2 endSpawningLocation = cvec2(endBlockSpawningOn.x() + 0.5, endBlockSpawningOn.y() + 1 + math::fpepsilon);
constexpr int endPlatformRadius = 0x2;
constexpr int endPlatformSpaceHeight = 0x4;

extern idList<block*, blockID> blockList;

rectangle2 entity::calculateHitBox() const
{
	return calculateHitBox(position);
}
int entity::getEffectLevel(const statusEffectID& id) const
{
	int highestPotency = 0;
	for (const statusEffect& effect : activeEffects)
	{
		if (effect.identifier == id)
		{
			highestPotency = math::maximum(effect.potency, highestPotency);
		}
	}
	return highestPotency;
}
vec2 entity::getRenderOffset(const renderData& targetData) const
{
	//when traveling to another dimension, don't interpolate positions
	return dimensionIn == newDimension ? targetData.getRenderOffset(position, newPosition) : position;
}
void entity::addSpeed(cvec2& additionalSpeed)
{
	speed += additionalSpeed;
}
vec2 entity::handleCollision(cvec2& otherSpeed, cfp& otherMass)
{
	cfp& weight = getWeight();
	//should collide directly ?
	//not a reference
	cvec2 oldSpeed = speed;
	speed = getSpeedAfterCollision(cvect2<vec2>(oldSpeed, otherSpeed), vec2(weight, otherMass));
	return getSpeedAfterCollision(cvect2<vec2>(otherSpeed, oldSpeed), vec2(otherMass, weight));
}
entity::entity(dimension* dimensionIn, cvec2& position, const entityID& entityType)
{
	this->entityType = entityType;
	this->dimensionIn = dimensionIn;
	this->position = position;
	newDimension = dimensionIn;
	newPosition = position;

	entityData* data = entityDataList[(int)entityType];

	health = getMaxHealth();
	relativeHitbox = data->initialRelativeHitbox;
}
void entity::tick()
{
	//remove old damage sources
	constexpr int damageRememberTime = 100;
	for (auto it = lastDamageSources.begin(); it != lastDamageSources.end(); it++)
	{
		if (it->get()->damageTick + damageRememberTime < currentWorld->ticksSinceStart)
		{
			it = lastDamageSources.erase(it);
			if (it == lastDamageSources.end())
			{
				break;
			}
		}
	}

	for (int i = 0; i < activeEffects.size(); i++)
	{
		statusEffect& effect = activeEffects[i];
		applyStatusEffect(effect);
		effect.ticksDuration--;
		if (effect.ticksDuration <= 0)
		{
			//remove
			if (effect.identifier == statusEffectID::absorption)
			{
				activeEffects.erase(activeEffects.begin() + i);
				fp maximalAbsorptionHealth = 0;
				for (int i = 0; i < activeEffects.size(); i++)
				{
					if (activeEffects[i].identifier == statusEffectID::absorption)
					{
						maximalAbsorptionHealth = math::maximum(maximalAbsorptionHealth, (fp)(activeEffects[i].potency * 4));
					}
				}
				absorptionHealth = math::minimum(absorptionHealth, maximalAbsorptionHealth);
			}
			else
			{
				activeEffects.erase(activeEffects.begin() + i);
			}
			i--;
		}
	}

	if (tasks)
	{
		tasks->execute();
	}

	crectangle2 currentHitbox = calculateHitBox(position);
	if (portalCoolDown)
	{
		portalCoolDown--;
	}

	if (entityDataList[(int)entityType]->canGoThroughPortals)
	{
		if (hitboxContainsOnly(currentHitbox, { blockID::portal }))
		{
			if (portalCoolDown == 0)
			{
				if (portalTicks > 80)
				{
					//travel to other dimension
					dimension* newDimension = dimensionIn->identifier == dimensionID::overworld ?
						currentWorld->dimensions[(int)dimensionID::nether] : currentWorld->dimensions[(int)dimensionID::overworld];
					//seek nether portal
					veci2 dimensionPosition;
					switch (newDimension->identifier)
					{
					case dimensionID::overworld:
					{
						dimensionPosition = floorVector(position * netherToOverWorldScale);
					}
					break;
					case dimensionID::nether:
					{
						dimensionPosition = floorVector(floorVector(position / netherToOverWorldScale));
					}
					break;
					}
					cveci2& portalBlockPosition = newDimension->searchPortal(dimensionPosition);
					cvec2& teleportPosition = cvec2(portalBlockPosition) + cvec2(0.5, math::fpepsilon);

					teleportTo(newDimension, teleportPosition, false);
					portalArriveSound->playRandomSound(newDimension, teleportPosition);
					portalCoolDown = 80;
				}
				else
				{
					if (portalTicks == 0)
					{
						portalDepartSound->playRandomSound(dimensionIn, position);
					}
					portalTicks++;
				}
			}
		}
		else if (hitboxContains(currentHitbox, { blockID::end_portal }))
		{
			if (newDimension->identifier == dimensionID::overworld)
			{
				//teleporting to the end
				teleportTo(currentWorld->dimensions[(int)dimensionID::end], endSpawningLocation, false);
				//generate obsidian platform
				newDimension->setBlockRange(endBlockSpawningOn + cveci2(-endPlatformRadius, 0), endBlockSpawningOn + cveci2(endPlatformRadius, 0), blockID::obsidian, chunkLoadLevel::entityLoaded);
				newDimension->setBlockRange(endBlockSpawningOn + cveci2(-endPlatformRadius, 1), endBlockSpawningOn + cveci2(endPlatformRadius, endPlatformSpaceHeight), blockID::air, chunkLoadLevel::entityLoaded);

				portalArriveSound->playRandomSound(newDimension, newPosition);
			}
			else
			{
				teleportTo(currentWorld->dimensions[(size_t)currentWorld->worldSpawnDimension], currentWorld->worldSpawnPoint, false);
				if (entityType == entityID::human && (!((human*)this)->seenCredits))
				{
					human* h = (human*)this;
					h->screen.startCredits = true;
					h->seenCredits = true;
				}
				else {
					portalArriveSound->playRandomSound(newDimension, newPosition);
				}
			}
		}
		else
		{
			portalTicks = 0;
		}
	}
	if (inBlocks())
	{
		//suffocate
		addDamageSource(1, std::make_shared<damageSource>(suffocationDamage));
	}

	bool addFireTicks = false;
	bool doubleDamage = false;
	if (hitboxContains(calculateHitBox(position), { blockID::soul_fire }))
	{
		addFireTicks = true;
		doubleDamage = true;
	}
	else if (hitboxContains(calculateHitBox(position), { blockID::fire }))
	{
		addFireTicks = true;
	}
	if (isUndeadMob(entityType) && (getVisibleSunLightLevel(dimensionIn->getInternalSunLightLevel(floorVector(position + cvec2(relativeHitbox.getCenter().x(), relativeHitbox.pos01().y())))) == maxLightLevel) && (fluidArea == 0))
	{
		if (!isHumanoid(entityType) || ((humanoid*)this)->armorSlots->slots[helmetArmorType - bootsArmorType].count == 0)
		{
			addFireTicks = true;
		}
	}

	if (fluidArea > 0)
	{
		if (!isImmuneToFire(entityType))
		{
			if (getFluidArea(calculateHitBox(position), { blockID::lava }) > 0)
			{
				addDamageSource(4, std::make_shared<damageSource>(fireDamage));
				cint initialLavaBurningTickCount = 300;
				if (fireTicks < initialLavaBurningTickCount)
				{
					fireTicks = initialLavaBurningTickCount;
				}
				addFireTicks = true;
			}
		}
		//dont add fire ticks when in water, but get damage from the lava
		if (getFluidArea(calculateHitBox(position), { blockID::water }) > 0)
		{
			if (fireTicks)
			{
				fireTicks = 0;
				if (!addFireTicks)
				{
					extinguishSound->playRandomSound(dimensionIn, position);
				}
			}
			addFireTicks = false;
		}
	}
	if (addFireTicks)
	{
		setOnFire(2);
	}
	if (fireTicks)
	{
		//each second
		if (currentWorld->ticksSinceStart % ticksPerRealLifeSecond == 0)
		{
			addDamageSource(doubleDamage ? 2 : 1, std::make_shared<damageSource>(fireDamage));
		}
		//for if a player respawned
		fireTicks = math::maximum(fireTicks - 1, 0);
	}
	if (immunityFrameCount)
	{
		immunityFrameCount--;
	}
	if (health <= 0)
	{
		onDeath();
	}
}
void entity::onDeath()
{
	despawn = true;
}
void entity::physics()
{
	bool newOnGround = false;
	cbool& oldOnGround = onGround;
	vec2 noCollisionPosition = newPosition + speed;
	vec2 positionAfterCollisions = newPosition;
	axisCollided = vect2<bool>();
	if (collideLevel != collisionTypeID::willNotCollide)
	{
		cvec2 oldSpeed = speed;

		fp secondsToCalculateLeft = secondsPerTick;
		do {
			//newOnGround = false;
			//check hitbox
			//relative to speed on collision axis?

			crectangle2 hitboxAfterCollisions = calculateHitBox(positionAfterCollisions);
			//cvec2 distanceTillEndOfTick = speed * secondsToCalculateLeft;
			collisionDataCollection data = dimensionIn->getRecursiveHitboxCollisionData(hitboxAfterCollisions, speed);
			data.evaluate(hitboxAfterCollisions, speed, secondsToCalculateLeft);

			//get real values
			constexpr fp maxStepHeight = 0.625;

			if (collideLevel == collisionTypeID::willCollideTop)
			{
				if (newOnGround)
				{
					int index = -1;
					fp firstCollisionTime = INFINITY;

					//check if another hitbox collided
					const fp hitbox00 = positionAfterCollisions.y() + relativeHitbox.y();

					for (int stepIndex = 0; stepIndex < data.hitboxes.size(); stepIndex++)
					{
						const collisionData& checkData = data.hitboxes[stepIndex];
						if ((checkData.collisionTime > math::fpepsilon) && (checkData.collisionTime < firstCollisionTime))
						{
							cfp collisionTop = checkData.hitboxCollidingWith.y() + checkData.hitboxCollidingWith.h();
							if ((checkData.type != collisionTypeID::willNotCollide) && (checkData.collisionTime > 0) && (checkData.collisionTime < secondsToCalculateLeft) && (collisionTop > hitbox00) && (collisionTop <= (hitbox00 + maxStepHeight)))
							{
								//check if covered
								collisionEdgeData edgeToCheck = collisionEdgeData({ cvec2(checkData.hitboxCollidingWith.x(),checkData.hitboxCollidingWith.w()) });
								edgeToCheck = edgeToCheck.substractCoveringEdges(data.getEdges(collisionTop, directionID::positiveY));

								if (edgeToCheck.edgeInRange(cvec2((speed.x() > checkData.speed.x()) ? checkData.hitboxCollidingWith.x() : ((checkData.hitboxCollidingWith.x() + checkData.hitboxCollidingWith.w()) - math::fpepsilon), math::fpepsilon)))
								{
									index = stepIndex;
									firstCollisionTime = checkData.collisionTime;
								}
							}
						}
					}
					cbool allowStepCollisionType[(size_t)collisionTypeID::count]
					{
						false, true, true
					};
					if (index != -1)
					{
						collisionData stepCollision = data.hitboxes[index];
						//step up block
						cfp hitboxHeight = stepCollision.hitboxCollidingWith.y() + stepCollision.hitboxCollidingWith.h();

						//stepped up position should be positionaftercollisions

						cvec2 steppedUpPosition = cvec2(speed.x() < stepCollision.speed.x() ? (stepCollision.hitboxCollidingWith.pos1() - relativeHitbox.pos0) : (stepCollision.hitboxCollidingWith.pos01()) - relativeHitbox.pos10()) + cvec2(0, math::fpepsilon);

						crectangle2 stepHitbox = calculateHitBox(steppedUpPosition);

						const collisionTypeID steppedCollisionType = dimensionIn->getHitboxCollisionType(stepHitbox);

						if (steppedCollisionType != collisionTypeID::willCollide)
						{
							speed.y() = stepCollision.speed.y();
							newOnGround = true;
							positionAfterCollisions = steppedUpPosition;
							secondsToCalculateLeft -= stepCollision.collisionTime;
							continue;//don't change tickpartleft and evaluate the rest of the time at this new elevation
						}
					}
				}
			}

			cbool allowCollisionType[(size_t)collisionTypeID::count]
			{
				false,
				false,
				true
			};
			collisionData firstCollision = data.getFirstCollision(allowCollisionType);

			if (firstCollision.collisionTime < secondsToCalculateLeft)
			{
				axisCollided = firstCollision.axisCollided;
				if (firstCollision.axisCollided.y())
				{
					newOnGround = true;
				}
			}
			if (collideLevel == collisionTypeID::willCollideTop)
			{
				const std::vector<collisionData> validCollisions = data.getCollisions(collisionTypeID::willCollideTop);
				for (const collisionData& collision : validCollisions)
				{
					if ((speed.y() < collision.speed.y()) && (collision.collisionTime < firstCollision.collisionTime && collision.collisionTime < secondsToCalculateLeft) &&
						((collision.hitboxCollidingWith.pos0.y() + collision.hitboxCollidingWith.h()) < positionAfterCollisions.y()))
					{
						crectangle2 collisionTopBorder = crectangle2(
							collision.hitboxCollidingWith.x(),
							collision.hitboxCollidingWith.y() + collision.hitboxCollidingWith.h(),
							collision.hitboxCollidingWith.w(), 0);
						//check if the border is not covered up by other blocks
						for (const collisionData& coverData : validCollisions)
						{
							crectangle2 coverHitbox = coverData.hitboxCollidingWith;
							/*covers partially:
							coverHitbox.x() < collisionTopBorder.x() + collisionTopBorder.w() &&
								coverHitbox.x() + coverHitbox.w() > collisionTopBorder.x() &&
								coverHitbox.y() <= collisionTopBorder.y() &&
								coverHitbox.y() + coverHitbox.h() > collisionTopBorder.y()
							*/
							//must cover completely
							if (coverHitbox.x() <= collisionTopBorder.x() &&
								coverHitbox.x() + coverHitbox.w() >= collisionTopBorder.x() + collisionTopBorder.w() &&
								coverHitbox.y() <= collisionTopBorder.y() &&
								coverHitbox.y() + coverHitbox.h() > collisionTopBorder.y()
								)
							{
								goto coveredUp;
							}
						}
						firstCollision = collision;
						newOnGround = true;
						axisCollided.y() = true;

					}
				coveredUp:;
				}
			}

			if (oldOnGround)
			{
				if (firstCollision.collisionTime < secondsToCalculateLeft)
				{
					//check if walking on a cactus
									//todo: better method than measuring the entire block height while keeping accuracy
					crectangle2& topHitbox = calculateHitBox(vec2(positionAfterCollisions.x(), positionAfterCollisions.y()));
					crectangle2& bottomHitbox = calculateHitBox(vec2(positionAfterCollisions.x(), noCollisionPosition.y()));

					cint left = (int)floor(topHitbox.x());
					cint right = (int)floor(topHitbox.x() + topHitbox.w());

					cint currentYLevel = (int)floor(topHitbox.y());
					cint newYLevel = (int)floor(bottomHitbox.y());

					cint fromYLevel = math::minimum(currentYLevel, newYLevel);
					cint toYLevel = math::maximum(currentYLevel, newYLevel);

					for (int checkYLevel = fromYLevel; checkYLevel <= toYLevel; checkYLevel++)
					{
						for (int checkX = left; checkX <= right; checkX++)
						{
							cveci2& checkPosition = cveci2(checkX, checkYLevel);

							collisionDataCollection collection = dimensionIn->getBlockCollisionData(checkPosition);
							collection.evaluate(hitboxAfterCollisions, speed, secondsToCalculateLeft);

							for (size_t hitboxIndex = 0; hitboxIndex < collection.hitboxes.size(); hitboxIndex++)
							{
								const collisionData currentData = collection.hitboxes[hitboxIndex];
								//stuck in a magma_block block will also damage you
								if ((currentData.collisionTime < (firstCollision.collisionTime + math::fpepsilon)) && ((currentData.type == collisionTypeID::willCollide) || (currentData.collisionTime > 0)))
								{
									collisionEdgeData edgeData = collisionEdgeData({ vec2(currentData.hitboxCollidingWith.x(), currentData.hitboxCollidingWith.w()) });
									edgeData = edgeData.substractCoveringEdges(data.getEdges(currentData.hitboxCollidingWith.pos01().y(), directionID::positiveY));

									if (edgeData.edges.size())
									{
										const blockID& blockBottom = dimensionIn->getBlockID(checkPosition);

										//collides
										if (blockBottom == blockID::cactus)
										{
											addDamageSource(1, std::make_shared<damageSource>(cactusDamage));
										}
										else if ((blockBottom == blockID::magma_block) && (!sneaking))
										{
											addDamageSource(1, std::make_shared<damageSource>(fireDamage));
										}
										else if (isPressurePlate(blockBottom))
										{
											if (isMob(entityType) || (!isStonePressurePlate(blockBottom)))
											{
												((pressurePlateData*)(dimensionIn->getBlockData(checkPosition)))->addEntityStandingOn(dimensionIn, checkPosition, this);
											}
										}
										break;
									}
								}
							}
						}
					}
				}
			}
			if (!newOnGround && sneaking && oldOnGround)
			{
				speed = vec2();//prevent from going off the edge
				positionAfterCollisions = newPosition;
				newOnGround = true;
				//continue, maybe a piston is pushing the sneaking person?
			}
			else if (firstCollision.collisionTime < secondsToCalculateLeft) {
				size_t axesImmediatelyCollided = 0;


				if (!oldOnGround && newOnGround)
				{
					onCollisionWithGround(speed.y() - firstCollision.speed.y());
				}
				positionAfterCollisions += speed * firstCollision.collisionTime;
				//multiply by friction
				for (size_t axisIndex = 0; axisIndex < 2; axisIndex++)
				{
					if (firstCollision.axisCollided[axisIndex])
					{
						if (firstCollision.collisionTime > 0)
						{
							//to avoid floating point errors
							if (speed[axisIndex] < firstCollision.speed[axisIndex])
							{
								positionAfterCollisions[axisIndex] += math::fpepsilon;
							}
							else if (speed[axisIndex] > firstCollision.speed[axisIndex])
							{
								positionAfterCollisions[axisIndex] -= math::fpepsilon;
							}
						}
						else
						{
							axesImmediatelyCollided++;
						}
						speed[axisIndex] = firstCollision.speed[axisIndex];
					}
				}
				secondsToCalculateLeft -= firstCollision.collisionTime * secondsToCalculateLeft;
				if (axesImmediatelyCollided == 2)
				{
					break;
				}
			}
			else {
				positionAfterCollisions += speed * secondsToCalculateLeft;
				break;
			}

			//if ((firstCollision.collisionTime >= secondsToCalculateLeft) || (secondsToCalculateLeft == 0) || (axesImmediatelyCollided == 2))
			//{
			//	break;
			//}
		} while (true);
	}
	else
	{
		positionAfterCollisions = noCollisionPosition;
	}
	crectangle2 blockCheckHitbox = calculateHitBox(position);
	newPosition = positionAfterCollisions;
	onGround = newOnGround;

	inCobweb = dimensionIn->blockRangeContains(cveci2((int)floor(blockCheckHitbox.x()), (int)floor(blockCheckHitbox.y())),
		cveci2((int)floor(blockCheckHitbox.x() + blockCheckHitbox.w()), (int)floor(blockCheckHitbox.y() + blockCheckHitbox.h())),
		{ blockID::cobweb });

	//check for fluids at the new position, BEFORE friction is applied, so you can jump out of low water streams with ease
	fluidArea = getFluidArea(blockCheckHitbox, std::vector<blockID>(fluidList, fluidList + fluidCount));

	speed.y() -= getGravityForce();

	//if (fluidArea > 0)
	//{
	//	cfp& fluidVolume = (fluidArea / blockCheckHitbox.size.volume()) * getVolume();
	//
	//	//fluid gives an upwards force of [volume in water] * 1000
	//	cfp& fluidDisplaceWeight = fluidVolume * blockList[blockID::water]->weightPerCubicMeter;
	//
	//	//add buoyancy
	//
	//
	//	speed = getSpeedAfterFriction(cvect2<vec2>(speed, cvec2(0, gravityForce)), cvec2(getWeight(), fluidDisplaceWeight));
	//}

	const std::vector<vec3>& frictions = getFrictions();

	for (vec3 fric : frictions)
	{
		speed = math::lerp(cvec2(fric), speed, fric.z()); //getSpeedAfterFriction(cvect2<vec2>(speed, cvec2(fric)), cvec2(getWeight(), fric.z()));
	}
}
bool entity::canTeleportTo(cvec2& position) const
{
	cfp maxFallDistance = 0x3;
	crectangle2 hitboxToTest = calculateHitBox(position);
	cvec2 distanceToTest = vec2(0, -maxFallDistance);
	collisionDataCollection data = dimensionIn->getHitboxCollisionData(hitboxToTest, distanceToTest);
	data.evaluate(hitboxToTest, distanceToTest, 1);
	constexpr bool allowCollisionType[(size_t)collisionTypeID::count]{ false, false, true };
	collisionData firstCollision = data.getFirstCollision(allowCollisionType);
	if (firstCollision.type != collisionTypeID::willCollide)
	{
		return false;
	}
	else
	{
		return firstCollision.collisionTime > 0;
	}
}

void entity::teleportRandomly()
{
	cint teleportTries = 0x40;
	cint maxTeleportDistance = 0x20;
	for (int i = 0; i < teleportTries; i++)
	{
		cvec2 teleportPosition = getRandomPointOnEllipse(currentRandom, crectangle2(position, vec2()).expanded(maxTeleportDistance));
		if (canTeleportTo(teleportPosition))
		{
			teleportTo(dimensionIn, teleportPosition, true);
			return;
		}
	}
}

//void entity::setAsPlayableCharachter()
//{
//	tasks = new playerControlledAI(this);
//	currentPlayableCharachter = this;
//	currentWorld->playableCharachterUUID = identifier;
//}

void entity::respawn()
{
	activeEffects = { statusEffect(statusEffectID::resistance,3 * ticksPerRealLifeSecond, 5) };
	teleportTo(currentWorld->dimensions[(int)currentWorld->worldSpawnDimension], currentWorld->worldSpawnPoint, false);
	health = getMaxHealth();
	fireTicks = 0;

	
	//currentPlayableCharachter-addStatusEffects();
	//currentPlayableCharachter->tasks = tasks;
	//currentPlayableCharachter->tasks->connectedEntity = currentPlayableCharachter;
	//currentPlayableCharachter->identifier = identifier;
	//tasks = nullptr;
	//return currentPlayableCharachter;
}

entity::~entity()
{
	if (tasks)
	{
		delete tasks;
	}
}
void entity::render(const renderData& targetData) const
{
	//render fire
	if (fireTicks)
	{
		crectangle2 hitboxRect = calculateHitBox(position);

		renderTexture(hitboxRect, targetData.worldToRenderTargetTransform, true, standardUpFacingBlockDirection, *blockList[(int)blockID::fire]->tex, targetData.renderTarget, true);
	}
}

void entity::onCollisionWithGround(cfp& verticalSpeed)
{
}

fp entity::getFluidArea(crectangle2& box, const std::vector<blockID>& fluids) const
{
	cveci2& pos0 = floorVector(box.pos0);
	crectanglei2& checkRect = crectanglei2(pos0, (floorVector(box.pos1()) - pos0) + 1);

	fp fluidArea = 0;

	for (cveci2& checkPos : checkRect)
	{
		blockID block = dimensionIn->getBlockID(checkPos);
		blockID fluid = block == blockID::kelp ? blockID::water : block;
		if (std::find(fluids.begin(), fluids.end(), fluid) != fluids.end())
		{
			rectangle2 fluidRect;
			if (block == fluid && checkPos.y() == checkRect.y())
			{
				fluidLevel level = ((fluidData*)dimensionIn->getBlockData(checkPos))->currentFluidLevel;

				fluidRect = crectangle2(cvec2(checkPos), cvec2(1, level / (fp)maxFluidLevel));
			}
			else
			{
				fluidRect = crectangle2(cvec2(checkPos), cvec2(1));
			}
			fluidArea += crectangle2::getCollisionArea(box, fluidRect);
		}
	}
	return fluidArea;
}

bool entity::hitboxContains(crectangle2& box, const std::vector<blockID>& blockIDArray)
{
	cint fromX = (int)floor(box.x());
	cint fromY = (int)floor(box.y());
	cvec2 topRight = box.pos1();
	cint toX = (int)floor(topRight.x());
	cint toY = (int)floor(topRight.y());
	//check hitbox for water
	veci2 checkPos = cveci2();
	for (checkPos.y() = fromY; checkPos.y() <= toY; checkPos.y()++)
	{
		for (checkPos.x() = fromX; checkPos.x() <= toX; checkPos.x()++)
		{
			blockID block = dimensionIn->getBlockID(checkPos);
			if (std::find(blockIDArray.begin(), blockIDArray.end(), block) != blockIDArray.end())
			{
				return true;
			}
		}
	}
	return false;
}
bool entity::hitboxContainsOnly(crectangle2& box, const std::vector<blockID>& blockIDArray)
{
	cint fromX = (int)floor(box.x());
	cint fromY = (int)floor(box.y());
	cvec2 topRight = box.pos1();
	cint toX = (int)floor(topRight.x());
	cint toY = (int)floor(topRight.y());
	//check hitbox for water
	veci2 checkPos = cveci2();
	for (checkPos.y() = fromY; checkPos.y() <= toY; checkPos.y()++)
	{
		for (checkPos.x() = fromX; checkPos.x() <= toX; checkPos.x()++)
		{
			blockID block = dimensionIn->getBlockID(checkPos);
			if (std::find(blockIDArray.begin(), blockIDArray.end(), block) == blockIDArray.end())
			{
				return false;
			}
		}
	}
	return true;
}

entity* createEntity(const entityID& entityType, dimension* dimensionIn, cvec2& position)
{
	switch (entityType)
	{
	case entityID::zombiefied_piglin:
		return new zombifiedPiglin(dimensionIn, position);
	case entityID::enderman:
		return new enderman(dimensionIn, position);
	case entityID::slime:
		return new slime(dimensionIn, position);
	case entityID::magma_cube:
		return new magmaCube(dimensionIn, position);
	case entityID::creeper:
		return new creeper(dimensionIn, position);
	case entityID::pig:
		return new pig(dimensionIn, position);
	case entityID::cow:
		return new cow(dimensionIn, position);
	case entityID::sheep:
		return new sheep(dimensionIn, position);
	case entityID::wolf:
		return new wolf(dimensionIn, position);
	case entityID::cod:
		return new cod(dimensionIn, position);
	case entityID::boat:
		return new boat(dimensionIn, position);
	case entityID::experience_orb:
		return new experienceOrb(dimensionIn, position);
	case entityID::item:
		return new floatingSlot(dimensionIn, position);
	case entityID::egg:
		return new egg(dimensionIn, position);
	case entityID::snowball:
		return new snowBall(dimensionIn, position);
	case entityID::ender_pearl:
		return new enderPearl(dimensionIn, position);
	case entityID::ender_eye:
		return new eyeOfEnder(dimensionIn, position);
	case entityID::fishing_bobber:
		return new fishingHook(dimensionIn, position);
	case entityID::arrow:
		return new arrow(dimensionIn, position);
	case entityID::ghast_fireball:
		return new ghastFireBall(dimensionIn, position);
	case entityID::dragon_fireball:
		return new dragonFireBall(dimensionIn, position);
	case entityID::small_Fireball:
		return new smallFireBall(dimensionIn, position);
	case entityID::tnt:
		return new tnt(dimensionIn, position);
	case entityID::end_crystal:
		return new endCrystal(dimensionIn, position);
	case entityID::falling_block:
		return new fallingBlock(dimensionIn, position);
	case entityID::shulker:
		return new shulker(dimensionIn, position);
	case entityID::blaze:
		return new blaze(dimensionIn, position);
	case entityID::ghast:
		return new ghast(dimensionIn, position);
	case entityID::ender_dragon:
		return new enderDragon(dimensionIn, position);
	case entityID::chicken:
		return new chicken(dimensionIn, position);
	case entityID::minecart:
		return new minecart(dimensionIn, position);
	case entityID::particle:
		return new particle(dimensionIn, position);
	case entityID::piston:
		return new piston(dimensionIn, position);
	case entityID::firework_rocket:
		return new fireworkRocket(dimensionIn, position);
	case entityID::pollen:
		return new pollen(dimensionIn, position);
	default:
		if (isZombie(entityType))
		{
			return new zombie(dimensionIn, position, entityType);
		}
		else if (isSkeleton(entityType))
		{
			return new skeleton(dimensionIn, position, entityType);
		}
		else
		{
			currentWorld->currentChat.addLine(std::wstring(L"summoning of this entity is not implemented"));
			return nullptr;
		}
		break;
	}
}

entity* summonEntity(const entityID& entityType, tickableBlockContainer* containerIn, cvec2& position)
{
	entity* const& e = createEntity(entityType, containerIn->rootDimension, containerIn->containerToRootTransform.multPointMatrix(position));
	e->addToWorld();
	return e;
}
entity* trySummonEntity(const entityID& entityType, tickableBlockContainer* containerIn, cvec2& position)
{
	entity* e = createEntity(entityType, containerIn->rootDimension, position);

	if (containerIn->rootDimension->meetsSpawningConditions(e))
	{
		e->addToWorld();
		return e;
	}
	else
	{

		delete e;
		return nullptr;
	}
}
//https://minecraft.gamepedia.com/Damage
bool entity::addDamageSource(cfp& damage, std::shared_ptr<damageSource> source)
{
	if (source.get()->type == damageType::fireDamage)
	{
		if (isImmuneToFire(entityType) || getEffectLevel(statusEffectID::fireResistance))
		{
			return false;
		}
	}

	cfp reducedDamageByEffects = damage * (1 - (getEffectLevel(statusEffectID::resistance) * 0.2));


	//can take damage while in invincibility frames, if it is higher
	//the highest damage source applies.

	cfp& totalHealth = health + absorptionHealth;
	fp oldTotalHealth = totalHealth;

	if (immunityFrameCount)
	{
		if (reducedDamageByEffects <= lastHitDamage)
		{
			return false;
		}
		oldTotalHealth += lastHitDamage;
	}

	lastHitDamage = reducedDamageByEffects;
	immunityFrameCount = immunityTime;

	cfp& newTotalHealth = oldTotalHealth - reducedDamageByEffects;
	absorptionHealth = math::maximum(absorptionHealth - reducedDamageByEffects, (fp)0);
	health = math::minimum(newTotalHealth, health);
	lastDamageSources.push_back(source);
	return true;
}

void entity::applyStatusEffect(const statusEffect& effect)
{
	if (effect.identifier == statusEffectID::poison)
	{
		cint ticks = math::maximum((int)(25 * pow(0.5, effect.potency)), 1);
		if ((currentWorld->ticksSinceStart % ticks) == 0)
		{
			if (health > 1)
			{
				addDamageSource(math::minimum((fp)1, health - 1), std::make_shared<damageSource>(potionDamage));
			}
		}
	}
	else if (effect.identifier == statusEffectID::regeneration)
	{
		cint ticks = math::maximum((int)(50 * pow(0.5, effect.potency)), 1);
		if ((currentWorld->ticksSinceStart % ticks) == 0)
		{
			heal(1);
		}
	}
}

void entity::addStatusEffects(const std::vector<statusEffect>& effectsToAdd)
{
	for (int i = 0; i < effectsToAdd.size(); i++)
	{
		const statusEffect& effect = effectsToAdd[i];
		if (effect.identifier == statusEffectID::instantHealth)
		{
			cfp& amount = 2 * pow(2, effect.potency);
			if (isUndeadMob(entityType))
			{
				addDamageSource(amount, std::make_shared<damageSource>(potionDamage));
			}
			else
			{
				heal(amount);
			}
		}
		else if (effect.identifier == statusEffectID::instantDamage)
		{
			cfp& amount = 3 * pow(2, effect.potency);
			if (isUndeadMob(entityType))
			{
				heal(amount);
			}
			else
			{
				addDamageSource(amount, std::make_shared<damageSource>(potionDamage));
			}
		}
		else
		{
			if (effect.identifier == statusEffectID::absorption)
			{
				cfp& newAbsorptionHealth = (fp)4 * effect.potency;
				if (newAbsorptionHealth > absorptionHealth)
				{
					cfp& difference = newAbsorptionHealth - absorptionHealth;
					absorptionHealth = newAbsorptionHealth;
				}
			}
			activeEffects.push_back(effect);
		}
	}
}

void entity::heal(cfp& health)
{
	this->health = math::minimum(this->health + health, entityDataList[(int)entityType]->maxHealth);
}
rectangle2 entity::calculateHitBox(const cvec2& pos) const
{
	return crectangle2(relativeHitbox.pos0 + pos, relativeHitbox.size);
}

void entity::serializeValue(nbtSerializer& s)
{
	//writing *this to the stream would be too risky
	//dont write despawn, type and position to the stream

	if (dimensionIn)
	{
		dimensionID newDimensionID;
		if (s.write)
		{
			newDimensionID = newDimension->identifier;
			s.serializeValue(std::wstring(L"new dimension"), (int&)newDimensionID);
		}
		else
		{
			if (s.serializeValue(std::wstring(L"new dimension"), (int&)newDimensionID))
			{
				newDimension = currentWorld->dimensions[(int)newDimensionID];
			}
		}
	}

	if (s.push<nbtDataTag::tagList>(std::wstring(L"active effects")))
	{
		if (s.write)
		{
			for (int i = 0; i < activeEffects.size(); i++)
			{
				if (s.push<nbtDataTag::tagCompound>())
				{
					activeEffects[i].serializeValue(s);
					s.pop();
				}
			}
		}
		else
		{
			std::vector<nbtData*> serializedEffectList = s.getChildren();
			activeEffects.clear();
			int i = 0;
			for (nbtData* serializedEffect : serializedEffectList)
			{
				if (s.push(serializedEffect))
				{
					statusEffect effect = statusEffect();
					effect.serializeValue(s);
					activeEffects.push_back(effect);
					s.pop();
				}
				i++;
			}
		}
		s.pop();
	}
	if (tasks && s.push<nbtDataTag::tagCompound>(std::wstring(L"tasks")))
	{
		tasks->serializeValue(s);
		s.pop();
	}

	s.serializeValue(std::wstring(L"identifier"), identifier);
	s.serializeValue(std::wstring(L"ticks standing in portal"), portalTicks);
	s.serializeValue(std::wstring(L"portal cooldown"), portalCoolDown);
	s.serializeValue(std::wstring(L"new position"), newPosition);
	s.serializeValue(std::wstring(L"speed"), speed);
	s.serializeValue(std::wstring(L"fire ticks"), fireTicks);
	s.serializeValue(std::wstring(L"immunity frame count"), immunityFrameCount);
	s.serializeValue(std::wstring(L"last hit damage"), lastHitDamage);
	s.serializeValue(std::wstring(L"health"), health);
	s.serializeValue(std::wstring(L"fluid area"), fluidArea);
	s.serializeValue(std::wstring(L"in cobweb"), inCobweb);
	s.serializeValue(std::wstring(L"sneaking"), sneaking);
	s.serializeValue(std::wstring(L"onGround"), onGround);
	s.serializeValue(std::wstring(L"collision type"), (int&)collideLevel);
	s.serializeValue(std::wstring(L"relative hitbox"), relativeHitbox);
}

bool entity::compareSelector(const human& sender, const std::wstring& selectorString) const
{
	if (selectorString == std::wstring(L"@e"))
	{
		return true;
	}
	return false;
}

void entity::teleportTo(dimension* newDimension, cvec2& newPosition, cbool& playTeleportSounds, cvec2& speedAfterTeleportation)
{
	this->newDimension = newDimension;
	this->newPosition = newPosition;
	//to avoid collisions
	speed = speedAfterTeleportation;
	if (dimensionIn != newDimension)
	{
		cveci2 newChunkCoordinates = getChunkCoordinates(newPosition);
		//load the chunk moved to
		newDimension->loadChunkIfNotLoaded(newChunkCoordinates, chunkLoadLevel::entityLoaded);
	}
	if (playTeleportSounds)
	{
		teleportFromSound->playRandomSound(dimensionIn, position);
		teleportToSound->playRandomSound(newDimension, newPosition);
	}
}

void entity::renderHitboxes(const renderData& targetData) const
{
	renderBlockRect(calculateHitBox(), targetData);
	cvec2 pos0 = targetData.worldToRenderTargetTransform.multPointMatrix(position);
	cvec2 pos1 = targetData.worldToRenderTargetTransform.multPointMatrix(newPosition);
	targetData.renderTarget.fillLine(pos0, pos1, solidColorBrush(colorPalette::blue));
}

bool entity::setOnFire(cint ticksToBurn)
{
	if (isImmuneToFire(entityType))
	{
		return false;
	}
	else
	{
		fireTicks += ticksToBurn;
		return true;
	}
}

fp entity::getMaxHealth() const
{
	return entityDataList[entityType]->maxHealth;
}

void entity::addToWorld()
{
	//when serializing an entity, uuids will be wasted, but whatever.
	identifier = randomUUID(currentRandom);
	chunk* chunkToAddTo = dimensionIn->loadChunkIfNotLoaded(getChunkCoordinates(position), chunkLoadLevel::worldGenerationLoaded);

	chunkToAddTo->entityList.push_back(this);
	//allowed, the fastlist is only used for moving entities efficiently
	chunkToAddTo->entityList.update();
}

fp entity::getGravityForce() const
{
	return gravityForce;
}

std::vector<vec3> entity::getFrictions() const
{
	std::vector<vec3> frictions = std::vector<vec3>();
	cfp& windArea = calculateHitBox().size.volume();
	cfp& windWeight = blockList[blockID::air]->weightPerCubicMeter * windArea;

	if (collideLevel == collisionTypeID::willNotCollide)
	{
		frictions.push_back(noCollisionFriction);
	}
	else
	{

		if (inCobweb)
		{
			frictions.push_back(cvec3(cvec2(), 0.5));
		}

		if (fluidArea > 0)
		{
			frictions.push_back(getFluidFriction());
		}

		if (onGround)
		{
			frictions.push_back(getGroundFriction());
		}

		cvec2& windSpeed = dimensionIn->getWindSpeed(calculateHitBox().getCenter());

		cfp& bodyWeight = getWeight();
		if (bodyWeight != INFINITY)
		{
			frictions.push_back(cvec3(windSpeed,
				bodyWeight == 0 ?
				bodyWeight :
				(bodyWeight / (windWeight + bodyWeight))
			));
		}
	}
	return frictions;
}

vec3 entity::getGroundFriction() const
{
	return cvec3(cvec2(), groundFrictionMultiplier);//cvec3(cvec2(), frictionPerBlock * getLengthTouchingGround());
}

vec3 entity::getFluidFriction() const
{
	return cvec3(cvec2(), fluidFrictionMultiplier); // 16 * (fluidArea / calculateHitBox().size.volume()));
}

fp entity::getLengthTouchingGround() const
{
	return onGround ? relativeHitbox.size.x() : 0;
}

fp entity::getWeight() const
{
	return entityDataList[entityType]->weight;
}

fp entity::getVolume() const
{
	return entityDataList[entityType]->volume;
}

bool entity::inBlocks() const
{
	return collideLevel != collisionTypeID::willNotCollide &&
		dimensionIn->getHitboxCollisionType(calculateHitBox(position)) == collisionTypeID::willCollide;
}
int getEntityIDByName(const std::wstring& name)
{
	for (int i = 0; i < (int)entityID::count; i++)
	{
		if (entityDataList[i]->name == name)
		{
			return i;
		}
	}
	return -1;
}

bool collidesThisTick(const entity& e1, const entity& e2)
{
	vect2<bool> axisCollided = vect2<bool>();
	collideTime2d(e1.calculateHitBox(), e2.calculateHitBox(), (e2.speed - e1.speed) * secondsPerTick, axisCollided);
	return axisCollided.x() || axisCollided.y();
}