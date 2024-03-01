#include "folderList.h"
#include "jigsawPool.h"
#include "include/filesystem/textfile.h"
#include "include/array/arrayfunctions.h"
std::vector<jigsawPool*> jigsawPoolList = std::vector<jigsawPool*>();
void reloadJigsawPools()
{
	jigsawPoolList = std::vector<jigsawPool*>();
	for (const auto& fileIterator : stdFileSystem::recursive_directory_iterator(structureFolder))
	{
		std::wstring path = fileIterator.path().wstring();
		std::wstring extension = fileIterator.path().extension().wstring();
		if (extension == jsonFileExtension)
		{
			jigsawPool* pool = jigsawPool::fromFile(path);
		}
	}
}
jigsawPool* jigsawPool::fromFile(const std::wstring path)
{
	if (stdFileSystem::exists(path))
	{
		jigsawPool* pool = new jigsawPool(stdFileSystem::path(path).replace_extension().wstring());

		const jsonContainer& content = readJson(stringToWString(readalltext(path)));
		const jsonContainer& elementContainer = content.children[content.getChildIndex(std::wstring(L"elements"))];
		for (int i = 0; i < elementContainer.children.size(); i++)
		{
			const jsonContainer& element = elementContainer.children[i];
			const jsonContainer& elementChild = element.children[element.getChildIndex(std::wstring(L"element"))];
			fp weight;
			if (convertToDouble(element.children[element.getChildIndex(std::wstring(L"weight"))].children[0].value, weight))
			{
				const std::wstring& relativeStructurePath = elementChild.children[elementChild.getChildIndex(std::wstring(L"location"))].children[0].value;

				pool->addTargetStructure(stdFileSystem::path(path).remove_filename().wstring() + std::wstring(L"\\"), relativeStructurePath, weight);
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

jigsawPool::jigsawPool(const std::wstring& name) :INamable(name)
{
}

void jigsawPool::addTargetStructure(const std::wstring& seekFolder, const std::wstring& structureName, cfp& weight)
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
		const jigsawPool* pool = getJigsawPoolByName(seekFolder + structureName);
		if (!pool)
		{
			pool = jigsawPool::fromFile(seekFolder + structureName + jsonFileExtension);
		}
		if (pool)
		{
			targetedStructures.insert(targetedStructures.end(), pool->targetedStructures.begin(), pool->targetedStructures.end());

			//add each weight to weights
			cfp weightMultiplier = 1.0 / getSum<fp>(pool->structureWeights);
			for (int i = 0; i < pool->structureWeights.size(); i++)
			{
				structureWeights.push_back(pool->structureWeights[i] * weightMultiplier);
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
jigsawPool* getJigsawPoolByName(std::wstring name)
{
	const stdFileSystem::path pathToSearch(name);
	auto it = std::find_if(jigsawPoolList.begin(), jigsawPoolList.end(),
		[pathToSearch](const auto& a) {
			return stdFileSystem::path(a->name).compare(pathToSearch) == 0;
		});

	return it == jigsawPoolList.end() ? nullptr : (*it);
}