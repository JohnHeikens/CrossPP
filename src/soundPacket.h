#pragma once
#include "socketContainer.h"
struct soundPacket {
	vec2 position;
	int soundCollectionID;
	int soundIndex;
	fp volume;
	fp pitch;
	soundPacket(cvec2& position = vec2(), cint& soundCollectionID = 0, cint& soundIndex = 0, cfp& volume = 1, cfp& pitch = 1) :
		position(position), soundCollectionID(soundCollectionID), soundIndex(soundIndex), volume(volume), pitch(pitch) {}
	bool serialize(const socketContainer& s);
};