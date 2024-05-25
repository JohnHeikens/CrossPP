// #include "soundHandler.h"
#include <SFML/Audio/Music.hpp>
#include "math/timemath.h"
#include "math/vector/vectn.h"
#include "filesystem/filemanager.h"
#include "array/fastlist.h"
#include "SFML/Audio/Sound.hpp"

#pragma once

extern int playingSoundCount;

constexpr fp maxVolume = 1;

class sfmlInputStream;

struct audio2d
{
	virtual void play() = 0;
	virtual void stop() = 0;
	microseconds startedPlaying = 0; // will be set by the first next update

	bool isSpatial = true;
	fp volume = 1;
	fp pitch = 1;
	vec2 pos = vec2();

	audio2d(cvec2 &pos, cfp &volume, cfp &pitch, cbool &isSpatial) : pos(pos), volume(volume), pitch(pitch), isSpatial(isSpatial), startedPlaying(0) {}

	virtual microseconds getDuration() = 0;
	virtual bool audioLoaded() const = 0;
	virtual void loadAudio() = 0;
	virtual void unLoadAudio() = 0;

	virtual void setVolume(cfp &volume) = 0;
	virtual void setPitch(cfp &pitch) = 0;
	virtual void setMinDistance(cfp &minDistance) = 0;
	virtual void setAttenuation(cfp &attenuation) = 0;
	virtual void setPlayingOffset(const microseconds &offset) = 0;
	virtual void setPosition(cvec2 &newPosition);
	virtual microseconds getPlayingOffset() = 0;

	virtual sf::SoundSource::Status getStatus() const = 0;
};

template <typename t>
struct audio2dt : audio2d, IDestructable
{
	t *playingAudio = nullptr;
	virtual bool audioLoaded() const override;
	virtual sf::SoundSource::Status getStatus() const override;
	virtual void unLoadAudio() override;

	virtual void setMinDistance(cfp &minDistance) override;
	virtual void setAttenuation(cfp &attenuation) override;
	virtual void setVolume(cfp &volume) override;
	virtual void setPitch(cfp &pitch) override;
	virtual void setPlayingOffset(const microseconds &offset) override;
	virtual microseconds getPlayingOffset() override;
	virtual void setPosition(cvec2 &newPosition) override;

	virtual void play() override;
	virtual void stop() override;

	audio2dt(cvec2 &pos, cfp &volume, cfp &pitch, cbool &isSpatial) : audio2d(pos, volume, pitch, isSpatial) {}

	inline ~audio2dt() override
	{
		delete playingAudio;
		playingAudio = nullptr;
	}
};

struct sound2d : audio2dt<sf::Sound>
{
	std::shared_ptr<sf::SoundBuffer> buffer;

	inline sound2d(const std::shared_ptr<sf::SoundBuffer> &buffer, cvec2 &pos, cfp &volume, cfp &pitch, cbool &isSpatial) : buffer(buffer), audio2dt(pos, volume, pitch, isSpatial)
	{
	}
	virtual void loadAudio() override;
	virtual microseconds getDuration() override;
};

struct music2d : audio2dt<sf::Music>
{
	stdPath path;
	sfmlInputStream *stream;

	inline music2d(const stdPath &path, cvec2 &pos, cfp &volume, cfp &pitch, cbool &isSpatial) : path(path), audio2dt(pos, volume, pitch, isSpatial)
	{
	}

	virtual void loadAudio() override;
	virtual microseconds getDuration() override;
	~music2d();
};

struct soundHandler2d : IDestructable
{
	fastList<std::shared_ptr<audio2d>> currentlyPlayIngAudio = fastList<std::shared_ptr<audio2d>>();
	// earPosition.z = the distance of the player to the screen in real life converted to 'ingame distance'
	void update(cvec3 &earPosition, cfp &hearingRange, cfp &maxVolume);

	void playAudio(const std::shared_ptr<audio2d> &audioToPlay);
	void stopAudio(const std::shared_ptr<audio2d> &audioToStop);
};

template <typename t>
inline bool audio2dt<t>::audioLoaded() const
{
	return playingAudio;
}

template <typename t>
inline sf::SoundSource::Status audio2dt<t>::getStatus() const
{
	assumeInRelease((bool)playingAudio);
	return playingAudio->getStatus();
}

template <typename t>
inline void audio2dt<t>::unLoadAudio()
{
	if (playingAudio)
	{
		playingAudio->stop();
		delete playingAudio;
		playingAudio = nullptr;
	}
}

template <typename t>
inline void audio2dt<t>::setMinDistance(cfp &minDistance)
{
	playingAudio->setMinDistance((float)minDistance);
}

template <typename t>
inline void audio2dt<t>::setAttenuation(cfp &attenuation)
{
	playingAudio->setAttenuation((float)attenuation);
}

template <typename t>
inline void audio2dt<t>::setVolume(cfp &volume)
{
	audio2d::volume = volume;
	if (playingAudio)
	{
		playingAudio->setVolume((float)volume * 100.0f);
	}
}
template <typename t>
inline void audio2dt<t>::setPitch(cfp &pitch)
{
	audio2d::pitch = pitch;
	if (playingAudio)
	{
		playingAudio->setPitch((float)pitch);
	}
}

template <typename t>
inline void audio2dt<t>::setPlayingOffset(const microseconds &offset)
{
	playingAudio->setPlayingOffset(sf::microseconds(offset));
}

template <typename t>
inline microseconds audio2dt<t>::getPlayingOffset()
{
	return playingAudio->getPlayingOffset().asMicroseconds();
}

template <typename t>
inline void audio2dt<t>::setPosition(cvec2 &newPosition)
{
	playingAudio->setPosition((float)newPosition.x, (float)newPosition.y, 0);
	audio2d::setPosition(newPosition);
}

template <typename t>
inline void audio2dt<t>::play()
{
	playingAudio->play();
	if (pitch != 1)
	{
		playingAudio->setPitch((float)pitch);
	}
	playingAudio->setVolume((float)volume * 100.0f);
	playingAudio->setPosition((float)pos.x, (float)pos.y, 0);
}

template <typename t>
inline void audio2dt<t>::stop()
{
	playingAudio->stop();
}

extern soundHandler2d *handler;