#pragma once
#include "levelID.h"
#include "blockID.h"
#include <climits>
#include "constants.h"
#include "lightLevel.h"

enum class arrayDataType : int
{
	blockIDType,
	blockDataType,
	levelType,
	count = ((int)levelType + (int)levelID::count)
};

constexpr int minArrayValue[(size_t)arrayDataType::count]
{
	0,
	0,
	0,
	0,
	0,
};
constexpr int maxArrayValue[(size_t)arrayDataType::count]
{
	(int)blockID::count,
	INT_MAX,
	maxLightLevel,
	maxLightLevel,
	maxPowerLevel,
};