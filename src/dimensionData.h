#pragma once
#include <memory>
#include "math/graphics/resolutiontexture.h"
#include "idList.h"
struct dimensionData
{
	dimensionData(const std::wstring& name, resolutionTexture* lightMapTexture, cbool& hasSunLight = false) :
		name(name), lightMapTexture(lightMapTexture), hasSunLight(hasSunLight) {}
	std::wstring name = L"";
	resolutionTexture* lightMapTexture = nullptr;
	bool hasSunLight = false;
};
extern idList<dimensionData*, dimensionID> dimensionDataList;