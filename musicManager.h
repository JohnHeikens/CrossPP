#pragma once
#include "musicCollection.h"
#include <memory>
void updateMusic(std::shared_ptr<musicCollection> const& musicIfFinished);
void stopMusic();

void updateMusic();
void replaceMusic(std::shared_ptr<musicCollection> const& newMusic);
void playMusic(std::shared_ptr<musicCollection> const& newMusic);

extern std::shared_ptr<music2d> currentlyPlayingMusic;
extern std::shared_ptr<musicCollection> currentlyPlayingCollection;