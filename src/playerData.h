#pragma once
#include "nbtSerializable.h"
struct playerData : nbtSerializable {
	uuid id;
	std::wstring name;
	virtual void serializeValue(nbtSerializer& s) override;
	bool serialize(cbool& write);
};