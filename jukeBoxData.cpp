#include "jukeBoxData.h"
#include "musicList.h"
bool jukeBoxData::tick(tickableBlockContainer* containerIn, cveci2& position)
{
	return false;
}

void jukeBoxData::onBlockRemove(tickableBlockContainer* containerIn, cveci2& position)
{
	recordSlot->dropContent(containerIn, cvec2(position) + 0.5);
	if (musicPlaying.get())
	{
		handler->stopAudio(musicPlaying);
	}
}

void jukeBoxData::serializeValue(nbtSerializer& s)
{
	recordSlot->serialize(s, std::wstring(L"record slot"));
	if (recordSlot->slots[0].count)
	{
		microseconds playProgress;
		if (s.write)
		{
			if (musicPlaying)
			{
				playProgress = musicPlaying->getPlayingOffset();
				s.serializeValue(std::wstring(L"play progress"), playProgress);
			}
		}
		else
		{
			if (s.serializeValue(std::wstring(L"play progress"), playProgress))
			{
				//playMusic(containerIn);
			}

			//todo: continue playing of music
		}
	}
}

jukeBoxData::~jukeBoxData()
{
	if (musicPlaying.get())
	{
		handler->stopAudio(musicPlaying);
	}

	delete recordSlot;
}

void jukeBoxData::playMusic(tickableBlockContainer* containerIn, cveci2& position)
{
	musicPlaying = recordsMusic->playAudio((int)recordSlot->slots[0].stackItemID - (int)itemID::music_disc, containerIn, cvec2(position) + 0.5);
}