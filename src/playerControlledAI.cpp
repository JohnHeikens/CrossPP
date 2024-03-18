#include "playerControlledAI.h"
#include "application/interaction.h"
#include "settings.h"
#include "math/collisions.h"
#include "boat.h"
#include "gameControl.h"
#include "idAnalysis.h"
#include "keyID.h"
#include "human.h"
#include "dimension.h"
#include "soundList.h"
#include "gameForm.h"
//to switch to flying or walking


void playerControlledAI::execute()
{
	human* connectedPlayer = (human*)connectedEntity;
	cbool& worldFocus = !(connectedPlayer->screen.focusedChild);// currentGame->focused && !currentGame->focusedChild;

	connectedPlayer->wantsToSprint = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::sprint);
	connectedPlayer->wantsToJump = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::jump);
	connectedPlayer->wantsToGoLeft = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::left);
	connectedPlayer->wantsToGoRight = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::right);
	connectedPlayer->wantsToSneak = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::sneak);
	connectedPlayer->wantsToGoDown = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::down);
	connectedPlayer->wantsToGoUp = worldFocus && connectedPlayer->screen.holdingDownKey((vk)keyID::up);

	cbool& spectating = connectedPlayer->currentGameMode == gameModeID::spectator;

	if (!spectating)
	{
		connectedPlayer->wantsToHit = worldFocus && connectedPlayer->screen.clickedFocused[mb::Left];
	}
	connectedPlayer->wantsToDig = worldFocus && (!spectating) && connectedPlayer->screen.holding[mb::Left];


	connectedPlayer->wantsToKeepUsing = worldFocus && connectedPlayer->screen.holding[mb::Right];
	connectedPlayer->wantsToStartUsing = worldFocus && connectedPlayer->screen.clickedFocused[mb::Right];
	connectedPlayer->wantsToStopUsing = worldFocus && connectedPlayer->screen.clickReleased[mb::Right];

	if (connectedPlayer->wantsToJump)
	{
		ticksSinceStoppedJumping++;
		if ((connectedPlayer->currentGameMode == gameModeID::creative) && (ticksSinceStoppedJumping <= ticksBetweenJumpsToFly))
		{
			connectedPlayer->flying = !connectedPlayer->flying;
		}
	}
	else
	{
		if (wasJumping)
		{
			ticksSinceStoppedJumping = 0;
		}
		else
		{
			ticksSinceStoppedJumping++;
		}
	}
	wasJumping = connectedPlayer->wantsToJump;


	if (connectedPlayer->UUIDRidingOn)
	{
		entity* entityRidingOn = connectedPlayer->dimensionIn->findUUID(connectedPlayer->position, ridingEntitySearchRadius, connectedPlayer->UUIDRidingOn);
		if (entityRidingOn)
		{
			if (entityRidingOn->entityType == entityID::boat)
			{
				boat* boatRidingOn = (boat*)entityRidingOn;
				if (entityRidingOn->fluidArea > 0)
				{
					if (connectedPlayer->wantsToGoLeft ^ connectedPlayer->wantsToGoRight)
					{
						boatRidingOn->paddleRotations += 0.5 * secondsPerTick;//each second forth and back
						if (math::isSummitBetween(boatRidingOn->paddleRotations, boatRidingOn->paddleRotations + 0.5 * secondsPerTick, [](cfp& x) {return -boat::getPaddleAngle(x); }))
						{
							waterPaddleSound->playRandomSound(boatRidingOn->dimensionIn, boatRidingOn->position);
						}
						cfp nextRotation = boatRidingOn->paddleRotations + 0.5 * secondsPerTick;

						cfp& paddleForce = boatSpeed * getFrictionMultiplier(entityRidingOn->getWeight(), entityRidingOn->fluidArea * waterWeightPerCubicMeter);

						if (connectedPlayer->wantsToGoLeft)
						{
							entityRidingOn->speed.x() -= paddleForce;
						}
						else if (connectedPlayer->wantsToGoRight)
						{
							entityRidingOn->speed.x() += paddleForce;
						}
					}
				}
			}
		}
	}
	if (worldFocus)
	{
		int scrollDelta = connectedPlayer->screen.scrollDelta;
		if (scrollDelta != 0)
		{
			if (connectedEntity->entityType == entityID::human)
			{
				if (connectedPlayer->screen.holdingDownKey((vk)keyID::camera))
				{
					connectedPlayer->screen.visibleRangeXWalk *= pow(0.95, scrollDelta);
				}
				connectedPlayer->rightHandSlotIndex = math::mod(connectedPlayer->rightHandSlotIndex - scrollDelta, StandardInventoryColumnCount);
			}
			//totalScrollDelta = 0;
		}
		if (connectedEntity->entityType == entityID::human)
		{
			for (keyHistoryEvent& event : connectedPlayer->screen.keyDownHistory) {
				for (int i = 0; i < 9; i++)
				{
					if (event.down && event.key == hotbarSlotKeys[i])
					{
						connectedPlayer->rightHandSlotIndex = i;
						break;
					}
				}
			}

			itemStack* selectedHotbarSlot = connectedPlayer->hotbarSlots->getSlot(cveci2(connectedPlayer->rightHandSlotIndex, 0));
			if (worldFocus && connectedPlayer->screen.clickedFocused[mb::Middle] && connectedPlayer->currentGameMode == gameModeID::creative)
			{
				//pick block
				selectedHotbarSlot->clearData();
				selectedHotbarSlot->stackItemID = (itemID)connectedPlayer->dimensionIn->getBlockID(connectedPlayer->selectedBlockPosition);
				selectedHotbarSlot->count = 1;
			}
			connectedPlayer->itemHolding = selectedHotbarSlot;
		}
	}
	connectedPlayer->flipBodyToLookingDirection();

}

void playerControlledAI::serializeValue(nbtSerializer& s)
{
	//we don't have to serialize tickssincestoppedjumping, that would only cause surprise moments where you f.e. stop flying when you reconnect because you're holding space

	//s.serializeValue(std::wstring(L"visible range x"), visibleRange.x());
	//s.serializeValue(std::wstring(L"visible range when walking"), visibleRangeXWalk);
}
