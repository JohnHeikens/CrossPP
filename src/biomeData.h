#pragma once
#include "math/graphics/color/color.h"
#include <string>
#include "idList.h"
#include "biomeID.h"

struct biomeData
{
	biomeData(const std::wstring& name, const color& grassColor = colorPalette::magenta) : name(name), grassColor(grassColor) {}
	color grassColor = color();
	std::wstring name;
};
extern idList<biomeData*, biomeID> biomeDataList;