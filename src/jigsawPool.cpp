#include "folderList.h"
#include "jigsawPool.h"
#include "filesystem/textfile.h"
#include "array/arrayfunctions.h"
std::vector<jigsawPool*> jigsawPoolList = std::vector<jigsawPool*>();
void reloadJigsawPools()
{
	jigsawPoolList = std::vector<jigsawPool*>();
	for (const auto& fileIterator : stdFileSystem::recursive_directory_iterator(structureFolder))
	{
		const stdPath& path = fileIterator.path();
		const stdPath& extension = path.extension().wstring();
		if (extension == jsonFileExtension)
		{
			jigsawPool* pool = jigsawPool::fromFile(path);
		}
	}
}
jigsawPool* jigsawPool::fromFile(const stdPath& path)
{
	if (stdFileSystem::exists(path))
	{
		jigsawPool* pool = new jigsawPool(stdPath(path).replace_extension());

		const jsonContainer& content = readJson(stringToWString(readAllText(path)));
		const jsonContainer& elementContainer = content.children[content.getChildIndex(std::wstring(L"elements"))];
		for (const jsonContainer& element : elementContainer.children)
		{
			const jsonContainer& elementChild = element.children[element.getChildIndex(std::wstring(L"element"))];
			fp weight;
			if (convertToDouble(element.children[element.getChildIndex(std::wstring(L"weight"))].children[0].value, weight))
			{
				const std::wstring& relativeStructurePath = elementChild.children[elementChild.getChildIndex(std::wstring(L"location"))].children[0].value;
				//create a copy and remove the filename of the copy
				pool->addTargetStructure(stdPath(path).remove_filename(), relativeStructurePath, weight);
			}
			else
			{
				handleError(std::wstring(L"undefined weight"));
			}
		}
		jigsawPoolList.push_back(pool);
		return pool;
	}
	else
	{
		return nullptr;
	}
}

jigsawPool::jigsawPool(const stdPath& path) :path(path)
{
}

void jigsawPool::addTargetStructure(const stdPath& seekFolder, const std::wstring& structureName, cfp& weight)
{
	std::vector<structure*> targetStructures = getStructuresByName(seekFolder, structureName);
	if (targetStructures.size())
	{
		cfp& dividedWeight = weight / targetStructures.size();
		for (structure* s : targetStructures)
		{
			targetedStructures.push_back(s);
			structureWeights.push_back(dividedWeight);
		}
	}
	else
	{
		const jigsawPool* pool = getJigsawPoolByPath(seekFolder / structureName);
		if (!pool)
		{
			pool = jigsawPool::fromFile(seekFolder / (structureName + jsonFileExtension));
		}
		if (pool)
		{
			targetedStructures.insert(targetedStructures.end(), pool->targetedStructures.begin(), pool->targetedStructures.end());

			//add each weight to weights
			cfp weightMultiplier = 1.0 / getSum<fp>(pool->structureWeights);
			for (const double& weight : pool->structureWeights)
			{
				structureWeights.push_back(weight * weightMultiplier);
			}
		}
	}
}

structure* jigsawPool::getRandomStructure(std::mt19937& randomToUse)
{
	if (targetedStructures.size())
	{
		return targetedStructures[randIndex(randomToUse, structureWeights, getSum<fp>(structureWeights))];
	}
	else
	{
		handleError(std::wstring(L"no structures in jigsaw pool"));
		return nullptr;
	}
}
jigsawPool* getJigsawPoolByPath(const stdPath& path)
{
	auto it = std::find_if(jigsawPoolList.begin(), jigsawPoolList.end(),
		[path](const auto& a) {
			return a->path.compare(path) == 0;
		});

	return it == jigsawPoolList.end() ? nullptr : (*it);
}