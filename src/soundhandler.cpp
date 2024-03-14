#include "soundHandler.h"
void soundHandler::update()
{
	for (int i = 0; i < currentlyPlayIngAudio->size; i++)
	{
		sf::Sound* s = (*currentlyPlayIngAudio)[i];
		if (s->getStatus() == sf::SoundSource::Status::Stopped)
		{
			//clear memory
			delete s;
			currentlyPlayIngAudio->erase(i);
		}
	}
	currentlyPlayIngAudio->update();
}

//returns the pointer of the sound, so you can always find it back
sf::Sound* soundHandler::playSound(sf::SoundBuffer* b, cfp& pitch, cfp& attenuation)
{
	sf::Sound* s = new sf::Sound(*b);
	if (pitch != 1) 
	{
		s->setPitch((float)pitch);
	}
	if (attenuation != 1)
	{
		s->setAttenuation((float)attenuation);
	}
	s->play();
	currentlyPlayIngAudio->push_back(s);
	return s;
}