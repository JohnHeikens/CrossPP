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
const musicCollection *currentlyPlayingCollection = nullptr;
void updateMusic(const musicCollection *const &musicIfFinished)
{
	if (!currentlyPlayingMusic || (currentlyPlayingMusic->audioLoaded() && currentlyPlayingMusic->getStatus() == sf::SoundSource::Status::Stopped) || !settings::soundSettings::playMusic)
	{
		stopMusic();
		if (settings::soundSettings::playMusic && musicIfFinished)
		{
			playMusic(musicIfFinished);
		}
	}
}
void stopMusic()
{
	if (currentlyPlayingMusic)
	{
		handler->stopAudio(currentlyPlayingMusic);
		//delete this reference
		currentlyPlayingMusic = nullptr;
	}
}
void replaceMusic(const musicCollection *const &newMusic)
{
	if (newMusic != currentlyPlayingCollection)
	{
		stopMusic();
		playMusic(newMusic);
	}
}
void playMusic(const musicCollection *const &newMusic)
{
	currentlyPlayingCollection = newMusic;
	currentlyPlayingMusic = newMusic->playRandomAudio();
}