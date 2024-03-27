#include "soundhandler2d.h"
int playingSoundCount = 0;

void soundHandler2d::update(cvec2& earPosition, cfp& hearingRange, cfp& playerHeadOffset, cfp& maxVolume)
{
	cvec3 earPosition3d = cvec3(earPosition.x, earPosition.y, playerHeadOffset);
	cfp hearingRange3d = cvec2(hearingRange, playerHeadOffset).length();
	sf::Listener::setPosition((float)earPosition3d.x, (float)earPosition3d.y, (float)earPosition3d.z);
	sf::Listener::setDirection(0.0f, 0.0f, -1.0f);//-z is at the back of the screen
	sf::Listener::setUpVector(0.0f, 1.0f, 0.0f);
	sf::Listener::setGlobalVolume((float)maxVolume * 100.0f);
	cmicroseconds currentTime = getmicroseconds();
	currentlyPlayIngAudio.update();
	cfp minimalVolume = 0.05f, maximalVolume = 1.0f;
	cfp distanceFullVolume3D = hearingRange3d * minimalVolume;

	for (size_t i = 0; i < currentlyPlayIngAudio.size; i++)
	{
		std::shared_ptr<audio2d> s = currentlyPlayIngAudio[i];

		cfp distance3D = (vec3(s->pos) - earPosition3d).length();
		cbool canHear = (distance3D <= hearingRange3d) || (!s->isSpatial);
		cfp volumeMultiplier = math::minimum(distanceFullVolume3D / distance3D, maximalVolume);

		const microseconds soundDuration = s->getDuration();

		if (s->startedPlaying)
		{
			//check if stopped
			if (s->audioLoaded())
			{
				if (s->getStatus() == sf::SoundSource::Status::Stopped)
				{
					//remove from list
					playingSoundCount--;
					currentlyPlayIngAudio.erase(i);
					continue;
				}
			}
			else if (s->startedPlaying + soundDuration < currentTime)
			{
				currentlyPlayIngAudio.erase(i);
				continue;
			}

		}
		else
		{
			s->startedPlaying = currentTime;
		}
		//check position
		if (s->audioLoaded() && (s->getStatus() == sf::SoundSource::Status::Playing))
		{
			if (!canHear)
			{
				playingSoundCount--;
				s->unLoadAudio();
				continue;
			}
		}
		else
		{
			//half the maximum
			if (canHear && (playingSoundCount < 0x100))
			{
				playingSoundCount++;
				if (!s->audioLoaded())
				{
					s->loadAudio();
				}
				s->play();
				s->setMinDistance(distanceFullVolume3D);
				s->setAttenuation(1.0 / distanceFullVolume3D);
				s->setPlayingOffset(currentTime - s->startedPlaying);
			}
			else
			{

			}
		}
		if (canHear)
		{
			//s->playingSound->setVolume(s->volume * volumeMultiplier * 100);
		}
	}
}

void soundHandler2d::playAudio(std::shared_ptr<audio2d> audioToPlay)
{
	currentlyPlayIngAudio.push_back(audioToPlay);

}

void soundHandler2d::stopAudio(std::shared_ptr < audio2d> audioToStop)
{
	currentlyPlayIngAudio.erase(audioToStop);
	audioToStop->unLoadAudio();
}

void sound2d::loadAudio()
{
	playingAudio = new sf::Sound(*buffer);
}

microseconds sound2d::getDuration()
{
	return buffer->getDuration().asMicroseconds();
}

void music2d::loadAudio()
{
	playingAudio = new sf::Music();
	if (!playingAudio->openFromFile(WStringToString(path)))
	{
		throw "file not found";
	}
}

microseconds music2d::getDuration()
{
	if (!audioLoaded())
	{
		loadAudio();
	}
	return playingAudio->getDuration().asMicroseconds();
}

void audio2d::setPosition(cvec2& newPosition)
{
	pos = newPosition;
}
