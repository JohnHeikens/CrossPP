#include "ridableEntity.h"
#include "dimension.h"
#include "mob.h"
#include "idAnalysis.h"
#include "math/collisions.h"
bool ridableEntity::addPassenger(mob* m)
{
	for (int i = 0; i < seats.size(); i++)
	{
		if (!seats[i])
		{
			seats[i] = m->identifier;
			m->UUIDRidingOn = identifier;
			return true;
		}
	}
	return false;
}

void ridableEntity::serializeValue(nbtSerializer& s)
{
	entity::serializeValue(s);
	if (s.push<nbtDataTag::tagList>(std::wstring(L"riding uuids")))
	{
		if (s.write)
		{
			for (int i = 0; i < seats.size(); i++)
			{
				if (s.push<nbtDataTag::tagCompound>())
				{
					s.serializeValue(std::wstring(L"uuid"), seats[i]);
					s.pop();
				}
			}
		}
		else
		{
			std::vector<nbtData*> data = s.getChildren();
			for (int i = 0; (i < seats.size()) && (i < data.size()); i++)
			{
				if (s.push(data[i]))
				{
					s.serializeValue(std::wstring(L"uuid"), seats[i]);
					s.pop();
				}
			}
		}
		s.pop();
	}
}

ridableEntity::ridableEntity(dimension* dimensionIn, cvec2& position, const entityID& entityType, cint& seatCount) : entity(dimensionIn, position, entityType), seats(std::vector<uuid>(seatCount))
{
}

void ridableEntity::addCollidingEntities()
{
	crectangle2 currentHitbox = calculateHitBox();
	const std::vector<entity*> possiblyCollidingEntities = dimensionIn->findNearEntities(currentHitbox.expanded(mobSizeMargin));

	for (int i = 0; i < possiblyCollidingEntities.size(); i++)
	{
		if (isMob(possiblyCollidingEntities[i]->entityType))
		{
			crectangle2 hitbox = possiblyCollidingEntities[i]->calculateHitBox();
			//entity must fit inside
			if (hitbox.size.x() < currentHitbox.size.x())
			{
				if (collides2d(hitbox, currentHitbox))
				{
					if (!addPassenger((mob*)possiblyCollidingEntities[i]))
					{
						//minecart full
						return;
					}
				}
			}
		}
	}
}
void ridableEntity::tick()
{
	entity::tick();
	for (int i = 0; i < seats.size(); i++)
	{
		if (seats[i])
		{
			//search passengers
			entity* ridingEntity = dimensionIn->findUUID(newPosition, 0x8 + mobSizeMargin, seats[i]);
			if (ridingEntity)
			{
				if (((mob*)ridingEntity)->UUIDRidingOn == identifier)
				{
					ridingEntity->newPosition = newPosition + getRelativeSeatPosition(i);
					ridingEntity->speed = speed;
				}
				else
				{
					seats[i] = uuid();
				}
			}
			else
			{
				seats[i] = uuid();
			}
		}
	}
}