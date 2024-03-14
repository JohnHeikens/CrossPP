#include "entityData.h"
idList<entityData*, entityID> entityDataList = idList<entityData*, entityID>();
entityData::entityData(const entityID& identifier, const std::wstring name, cfp& maxHealth, crectangle2& initialialRelativeHitbox, cfp& volume, cfp& weight, cbool& canGoThroughPortals)
	: identifier(identifier), name(name), maxHealth(maxHealth), initialRelativeHitbox(initialialRelativeHitbox), canGoThroughPortals(canGoThroughPortals), volume(volume), weight(weight)
{
}