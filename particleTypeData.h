#pragma once
#include "particleID.h"
#include "include/interface/inamable.h"
#include "idList.h"
struct particleTypeData : INamable
{
	particleTypeData(const std::wstring& name) : INamable(name) {}
};
extern idList<particleTypeData*, particleID> particleTypeDataList;