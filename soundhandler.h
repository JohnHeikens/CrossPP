#pragma once
#include "SFML/Audio.hpp"
#include "include/GlobalFunctions.h"
#include "include/array/fastlist.h"
struct soundHandler
{
	fastList<sf::Sound*>* currentlyPlayIngAudio;
	void update();
	sf::Sound* playSound(sf::SoundBuffer* b, cfp& pitch = 1, cfp& attenuation = 1);
	soundHandler() :currentlyPlayIngAudio(new fastList<sf::Sound*>()) {}
};
