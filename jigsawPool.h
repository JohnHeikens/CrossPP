#pragma once
#include "include/interface/inamable.h"
#include <vector>
#include "structure.h"
struct jigsawPool :INamable
{
	std::vector<structure*> targetedStructures;
	std::vector<fp> structureWeights;
	static jigsawPool* fromFile(const std::wstring path);
	jigsawPool(const std::wstring& name);
	void addTargetStructure(const std::wstring& seekFolder, const std::wstring& structureName, cfp& weight);
	structure* getRandomStructure(std::mt19937& randomToUse);
};

void reloadJigsawPools();
jigsawPool* getJigsawPoolByName(std::wstring name);
extern std::vector<jigsawPool*> jigsawPoolList;