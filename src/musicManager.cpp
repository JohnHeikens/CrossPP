#include "world.h"
#include "idAnalysis.h"
#include "entity.h"
#include "settings.h"
#include "musicManager.h"
#include "dimension.h"
#include "human.h"
#include "end.h"
#include "musicList.h"
std::shared_ptr<music2d> currentlyPlayingMusic = nullptr;
std::shared_ptr<musicCollection> currentlyPlayingCollection = nullptr;
void updateMusic(std::shared_ptr<musicCollection> const& musicIfFinished)
{
	if (currentlyPlayingMusic == nullptr || (currentlyPlayingMusic && currentlyPlayingMusic->audioLoaded() && currentlyPlayingMusic->getStatus() == sf::SoundSource::Status::Stopped) || !settings::soundSettings::playMusic)
	{
		stopMusic();
		if (settings::soundSettings::playMusic && musicIfFinished)
		{
			playMusic(musicIfFinished);
		}
		else
		{
			currentlyPlayingMusic = nullptr;
		}
	}
}
void stopMusic()
{
	if (currentlyPlayingMusic)
	{
		handler->stopAudio(currentlyPlayingMusic);
		currentlyPlayingMusic = nullptr;
	}
}
void replaceMusic(std::shared_ptr<musicCollection> const& newMusic)
{
	stopMusic();
	playMusic(newMusic);
}
void playMusic(std::shared_ptr<musicCollection> const& newMusic)
{
	currentlyPlayingCollection = newMusic;
	currentlyPlayingMusic = newMusic->playRandomAudio();
}
void updateMusic()
{
	//bool boatInWater;
	//if (isMob(currentPlayableCharachter->entityType))
	//{
	//	mob* currentMob = (mob*)currentPlayableCharachter;
	//	entity* entityRidingOn = currentPlayableCharachter->dimensionIn->findUUID(currentPlayableCharachter->position, ridingEntitySearchRadius, currentMob->UUIDRidingOn);
	//	if (entityRidingOn && entityRidingOn->entityType == entityID::boat && (entityRidingOn->getFluidArea(entityRidingOn->calculateHitBox(), { blockID::water }) > 0))
	//	{
	//		boatInWater = true;
	//	}
	//	else
	//	{
	//		//did not find the boat
	//		boatInWater = false;
	//	}
	//}
	//else
	//{
	//	boatInWater = false;
	//}
	//
	//cbool inEnd = currentPlayableCharachter->dimensionIn->identifier == dimensionID::end;
	//if (inEnd && ((end*)currentWorld->dimensions[(int)dimensionID::end])->dragonAlive)
	//{
	//	if (currentlyPlayingCollection != bossMusic)
	//	{
	//		replaceMusic(bossMusic);
	//	}
	//}
	//else
	//{
	//	const biomeID biomeIn = currentPlayableCharachter->dimensionIn->getBiome(currentPlayableCharachter->position);
	//	cbool inNether = currentPlayableCharachter->dimensionIn->identifier == dimensionID::nether;
	//	if (inNether)
	//	{
	//		if (biomeIn == biomeID::soulsand_valley)
	//		{
	//			updateMusic(soulSandValleyMusic);
	//		}
	//		else if (biomeIn == biomeID::nether_wastes)
	//		{
	//			updateMusic(netherWastesMusic);
	//		}
	//		else if (biomeIn == biomeID::warped_forest)
	//		{
	//			updateMusic(nullptr);
	//		}
	//		else if (biomeIn != biomeID::warped_forest)
	//		{
	//			updateMusic(netherMusic);
	//		}
	//	}
	//	else
	//	{
	//		cbool inWater = (currentPlayableCharachter->getFluidArea(currentPlayableCharachter->calculateHitBox(), { blockID::water }) > 0);
	//		cbool inOcean = currentPlayableCharachter->dimensionIn->getBiome(currentPlayableCharachter->position) == biomeID::ocean;
	//		cbool inCreative = (currentPlayableCharachter->entityType == entityID::human) && ((human*)currentPlayableCharachter)->currentGameMode == gameModeID::creative;
	//		updateMusic(inNether ? netherMusic :
	//			inEnd ? endMusic :
	//			((boatInWater || inWater) && inOcean) ? waterMusic :
	//			inCreative ? creativeModeMusic :
	//			overWorldBackgroundMusic);
	//	}
	//}
}