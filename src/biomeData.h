#pragma once
#include "math/graphics/color/color.h"
#include <string>
#include "idList.h"
#include "biomeID.h"

struct biomeData
{
	color grassColor = color();
	color biomeColor;
	std::wstring name;
	biomeData(const std::wstring& name, const color& biomeColor, const color& grassColor) : name(name), biomeColor(biomeColor), grassColor(grassColor) {}
};
extern idList<biomeData*, biomeID> biomeDataList;