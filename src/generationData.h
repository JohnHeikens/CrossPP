#pragma once
#include "interface/idestructable.h"
#include "nbtSerializable.h"
struct generationData :IDestructable, nbtSerializable
{
	virtual void serializeValue(nbtSerializer& s) override;
};