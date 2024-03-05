#pragma once
#include "particleID.h"
#include "include/interface/inamable.h"
#include "idList.h"
struct particleTypeData : INamable
{
	particleTypeData(const std::wstring& name);
	std::vector<resolutionTexture*> textures;
};
extern idList<particleTypeData*, particleID> particleTypeDataList;