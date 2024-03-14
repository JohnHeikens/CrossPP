#pragma once
#include "interface/idestructable.h"
#include "nbtSerializable.h"
#include "itemID.h"
struct itemTag : nbtSerializable, IDestructable
{
	virtual void serializeValue(nbtSerializer& s) override;
	virtual ~itemTag() override;
};

itemTag* createItemTag(const itemID& id);