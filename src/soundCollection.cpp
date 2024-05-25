#include "soundCollection.h"
#include "constants.h"
#include "tickableBlockContainer.h"
#include "server.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <SFML/Audio/SoundBuffer.hpp>
#include "audioCollection.h"
#include "array/wstringFunctions.h"
#include "globalFunctions.h"
#include "math/random/random.h"
#include "math/vector/vectn.h"
#include "soundHandler2D.h"
#include "soundPacket.h"
#include "human.h"
#include <fstream>
#include "filesystem/fileio.h"
#include "filesystem/textfile.h"
#include "filesystem/sfmlInputStream.h"
#include "folderList.h"
#include <regex>
#include "resourcePack.h"

std::unordered_map<std::wstring, audioCollection *> globalSoundCollectionList = std::unordered_map<std::wstring, audioCollection *>();

soundCollection::soundCollection()
{
	audioToChooseFrom = std::vector<std::shared_ptr<sf::SoundBuffer>>();
}

soundCollection::soundCollection(const stdPath &path) : soundCollection()
{
	addAudioFileName(path);
}

void soundCollection::addSoundCollection(const soundCollection &collection)
{
	audioToChooseFrom.insert(audioToChooseFrom.end(), collection.audioToChooseFrom.begin(), collection.audioToChooseFrom.end());
}

void audioCollection::addAudioFileName(const stdPath &path)
{
	key = path.lexically_relative(generalSoundFolder).wstring(); // - generalSoundFolder;

	key = std::regex_replace(key, std::wregex(L"\\/|\\\\"), L".");
	// key.replace(key.begin(), key.end(), stdPath::preferred_separator, L'.');

	globalSoundCollectionList[key] = this;
	bool loaded = false;
	stdPath defaultPath = path;
	defaultPath += L".ogg";

	stdPath foundPath;
	if (getLastResourceLocation(defaultPath, foundPath))
	{
		addAudioFile(foundPath);
		loaded = true;
	}
	// warning: starts with 1
	int i = 1;
	while (true)
	{
		stdPath pathNumber = path;
		pathNumber += std::to_wstring(i) + L".ogg";
		if (getLastResourceLocation(pathNumber, foundPath))
		{
			addAudioFile(foundPath);
			loaded = true;
		}
		else
			break;
		i++;
	}
	if (!loaded)
	{
		handleError(std::wstring(L"no sound found in " + path.wstring()));
	}
}

void audioCollection::addAudioFile(const stdPath &path)
{
	handleError(std::wstring(L"not implemented"));
}

void soundCollection::addAudioFile(const stdPath &path)
{
	const auto buffer = std::make_shared<sf::SoundBuffer>();
	// this way, even on android we can read from an actual file instead of the internal APK storage
	auto stream = sfmlInputStream(std::make_shared<std::ifstream>(path, getOpenMode(false)));

	// std::string data = readAllText(path);

	// std::ifstream s(path, getOpenMode(false));

	// s.write(text.c_str(), (std::streamsize)text.size());
	// s.close();
	// str = std::ifstream(path,fileOpenMode::read)
	// sf::FileInputStream str;
	// str.open(path);

	// sf::InputStream str
	buffer->loadFromStream(stream);
	audioToChooseFrom.push_back(buffer);
}

std::shared_ptr<audio2d> soundCollection::playRandomSound(tickableBlockContainer *containerIn, cvec2 &position, cfp &volume, cfp &pitch)
{
	return playSound(randIndex(currentRandom, audioToChooseFrom.size()), containerIn, position, volume, pitch);
}

std::shared_ptr<audio2d> soundCollection::playRandomSound(cfp &volume, cfp &pitch)
{
	return playSound(randIndex(currentRandom, audioToChooseFrom.size()), volume, pitch);
}

std::shared_ptr<sound2d> soundCollection::playSound(csize_t &index, tickableBlockContainer *containerIn, cvec2 &position, cfp &volume, cfp &pitch)
{
	const dimension *soundDimension = containerIn->rootDimension;
	cvec2 &soundPosition = containerIn->containerToRootTransform.multPointMatrix(position);
	const auto &players = currentServer->getPlayersInRadius(soundDimension, soundPosition, soundLoadRange);
	soundPacket packet = soundPacket(position, key, (int)index, volume, pitch);
	for (const auto &p : players)
	{
		p->screen.dataToSend.push_back(packet);
	}

	return nullptr;
}

std::shared_ptr<sound2d> soundCollection::playSound(csize_t &index, cfp &volume, cfp &pitch)
{
	std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(audioToChooseFrom[index], cvec2(), volume, pitch, false);
	handler->playAudio(soundToPlay);
	return soundToPlay;
}