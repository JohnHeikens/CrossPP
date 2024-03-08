#pragma once
#include <string>
//folders
const std::wstring dataFolder = std::wstring(L"data\\");

const std::wstring miscellaneousDataFolder = dataFolder + std::wstring(L"miscellaneous\\");
const std::wstring miscellaneousTexturesFolder = miscellaneousDataFolder + std::wstring(L"textures\\");
const std::wstring miscellaneousMusicFolder = miscellaneousDataFolder + std::wstring(L"music\\");
const std::wstring miscellaneousNetherMusicFolder = miscellaneousMusicFolder + std::wstring(L"nether\\");
const std::wstring fileVersionsFolder = miscellaneousDataFolder + std::wstring(L"fileversions\\");
const std::wstring playerDataFolder = dataFolder + std::wstring(L"player\\");
const std::wstring skinFolder = playerDataFolder + std::wstring(L"skins\\");

const std::wstring resourcePackFolder = dataFolder + std::wstring(L"resourcepacks\\");

const std::wstring assetsFolder = std::wstring(L"assets\\minecraft\\");
const std::wstring optifineFolder = assetsFolder + std::wstring(L"optifine\\");
const std::wstring particleDataFolder = assetsFolder + std::wstring(L"particles\\");
const std::wstring generalTextureFolder = assetsFolder + std::wstring(L"textures\\");
const std::wstring particleTexturesFolder = generalTextureFolder + std::wstring(L"particle\\");
const std::wstring guiTextureFolder = generalTextureFolder + std::wstring(L"gui\\");
const std::wstring blockTextureFolder = generalTextureFolder + std::wstring(L"block\\");
const std::wstring environmentTextureFolder = generalTextureFolder + std::wstring(L"environment\\");
const std::wstring containerTextureFolder = generalTextureFolder + std::wstring(L"gui\\container\\");
const std::wstring entityTextureFolder = generalTextureFolder + std::wstring(L"entity\\");
const std::wstring itemTextureFolder = generalTextureFolder + std::wstring(L"item\\");
const std::wstring armorTextureFolder = generalTextureFolder + std::wstring(L"models\\armor\\");
const std::wstring miscellaneousTextureFolder = generalTextureFolder + std::wstring(L"misc\\");
const std::wstring lightMapFolder = optifineFolder + std::wstring(L"lightmap\\");
const std::wstring particleTextureFolder = generalTextureFolder + std::wstring(L"particle\\");

const std::wstring generalSoundFolder = dataFolder + std::wstring(L"sounds\\1.16\\minecraft\\sounds\\");
const std::wstring blockSoundFolder = generalSoundFolder + std::wstring(L"block\\");
const std::wstring tileSoundFolder = generalSoundFolder + std::wstring(L"tile\\");
const std::wstring itemSoundFolder = generalSoundFolder + std::wstring(L"item\\");
const std::wstring entitySoundFolder = generalSoundFolder + std::wstring(L"entity\\");
const std::wstring mobSoundFolder = generalSoundFolder + std::wstring(L"mob\\");
const std::wstring miscellaneousSoundFolder = generalSoundFolder + std::wstring(L"random\\");
const std::wstring generalMusicFolder = generalSoundFolder + std::wstring(L"music\\");
const std::wstring gameMusicFolder = generalMusicFolder + std::wstring(L"game\\");
const std::wstring menuMusicFolder = generalMusicFolder + std::wstring(L"menu\\");
const std::wstring waterMusicFolder = gameMusicFolder + std::wstring(L"water\\");
const std::wstring creativeModeMusicFolder = gameMusicFolder + std::wstring(L"creative\\");
const std::wstring netherMusicFolder = gameMusicFolder + std::wstring(L"nether\\");
const std::wstring endMusicFolder = gameMusicFolder + std::wstring(L"end\\");
const std::wstring recordsMusicFolder = generalSoundFolder + std::wstring(L"records\\");
const std::wstring noteSoundFolder = generalSoundFolder + std::wstring(L"note\\");
const std::wstring extraSoundFolder = dataFolder + std::wstring(L"sounds\\extra\\");

const std::wstring savesFolder = dataFolder + std::wstring(L"saves\\");
const std::wstring serversFolder = dataFolder + std::wstring(L"servers\\");
const std::wstring saveFolderDimensions = std::wstring(L"dimensions\\");
const std::wstring saveFolderChunks = std::wstring(L"chunks\\");
const std::wstring saveFolderScreenShot = std::wstring(L"screenshot.png");
const std::wstring saveFolderWorldData = std::wstring(L"world data");

const std::wstring minecraftVersionFolder = dataFolder + std::wstring(L"version\\1.16.2\\");
const std::wstring minecraftDataFolder = minecraftVersionFolder + std::wstring(L"data\\minecraft\\");
const std::wstring minecraftAssetsFolder = minecraftVersionFolder + std::wstring(L"assets\\minecraft\\");
const std::wstring mainTagFolder = minecraftDataFolder + std::wstring(L"tags\\");
const std::wstring recipeFolder = minecraftDataFolder + std::wstring(L"recipes\\");
const std::wstring dataPacksFolder = dataFolder + std::wstring(L"data packs\\");
const std::wstring lootTablesFolder = minecraftDataFolder + std::wstring(L"loot_tables\\");
const std::wstring blockLootTablesFolder = lootTablesFolder + std::wstring(L"blocks\\");
const std::wstring chestLootTablesFolder = lootTablesFolder + std::wstring(L"chests\\");
const std::wstring entityLootTablesFolder = lootTablesFolder + std::wstring(L"entities\\");
const std::wstring structureFolder = dataFolder + std::wstring(L"structures\\");
const std::wstring textFolder = minecraftAssetsFolder + std::wstring(L"texts\\");
extern std::filesystem::path workingDirectory;