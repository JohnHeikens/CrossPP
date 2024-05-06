#pragma once
#include "interface/idestructable.h"
#include "entityID.h"
#include <string>
#include "math/rectangle/rectangletn.h"
#include "idList.h"
struct entityData :IDestructable
{
	entityData(const entityID& identifier, const std::wstring name, cfp& maxHealth, crectangle2& initialialRelativeHitbox, cfp& volume = 0, cfp& weight = 0, cbool& canGoThroughPortals = true);
	std::wstring name = std::wstring(L"");
	fp maxHealth = 0;
	entityID identifier = (entityID)0;
	rectangle2 initialRelativeHitbox = crectangle2();
	bool canGoThroughPortals = true;
	fp weight = 0;//weight in kilograms
	fp volume = 0;//the volume of the actual entity (not the hitbox volume)
};
extern idList<entityData*, entityID> entityDataList;