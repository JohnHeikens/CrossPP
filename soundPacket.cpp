#include "soundPacket.h"

bool soundPacket::serialize(const socketContainer& s)
{
	s.serialize(position);
	s.serialize(soundCollectionID);
	s.serialize(soundIndex);
	s.serialize(volume);
	return s.serialize(pitch);
}
