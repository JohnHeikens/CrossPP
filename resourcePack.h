#pragma once
#include "constants.h"
#include "include/math/graphics/resolutiontexture.h"
#include <memory>
#include "itemID.h"
#include "soundCollection.h"

resolutionTexture* loadTextureFromResourcePack(const std::wstring& relativePath, cbool& addToTextureList = true);
resolutionTexture* loadTexture(std::wstring path, cveci2& defaultSize = cveci2(blockTextureSize), cbool& addToTextureList = true);
resolutionTexture* loadChestTexture(const std::wstring& path);
void loadResourcePacks();
std::vector<std::wstring> getResourceLocations(const std::wstring& relativePath);

std::vector<resolutionTexture*> loadGrowthStageTextures(const std::wstring& blockName, std::vector<int> growthStageIndexes);
resolutionTexture* loadRotatedTexture(const std::wstring& path, cvec2& defaultSize, cint& angle);
resolutionTexture* loadRailsTexture(const std::wstring& path);

std::shared_ptr<soundCollection> getEquipSound(const itemID& identifier);
void unloadResourcePacks();

extern std::vector<std::wstring> resourcePackPaths;
extern std::wstring creditsText;
extern std::vector<resolutionTexture*> loadedTextures;


extern fp guiTextureMultiplier;