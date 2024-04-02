#pragma once
#include "constants.h"
#include "math/graphics/resolutiontexture.h"
#include <memory>
#include "itemID.h"
#include "soundCollection.h"
#include "filesystem/filemanager.h"

resolutionTexture* loadTextureFromResourcePack(const stdPath& relativePath, cbool& addToTextureList = true);
resolutionTexture* loadTexture(stdPath path, cveci2& defaultSize = cveci2(blockTextureSize), cbool& addToTextureList = true);
resolutionTexture* loadChestTexture(const stdPath& path);
void loadResourcePacks();
std::vector<stdPath> getResourceLocations(const stdPath& relativePath);

std::vector<resolutionTexture*> loadGrowthStageTextures(const std::wstring& blockName, std::vector<int> growthStageIndexes);
resolutionTexture* loadRotatedTexture(const stdPath& path, cvec2& defaultSize, cint& angle);
resolutionTexture* loadRailsTexture(const stdPath& path);

std::shared_ptr<soundCollection> getEquipSound(const itemID& identifier);
void unloadResourcePacks();

extern std::vector<stdPath> resourcePackPaths;
extern std::wstring creditsText;
extern std::vector<resolutionTexture*> loadedTextures;


extern fp guiTextureMultiplier;