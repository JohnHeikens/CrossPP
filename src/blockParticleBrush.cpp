#include "block.h"
#include "blockParticleBrush.h"
#include "nbtSerializer.h"
blockParticleBrush::blockParticleBrush() :rectParticleBrush(particleID::block)
{
}

blockParticleBrush::blockParticleBrush(const blockID& brushBlockID) : rectParticleBrush(particleID::block, getParticleBrushRect(*blockList[brushBlockID]->tex)), brushBlockID(brushBlockID)
{
}


void blockParticleBrush::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"brush block id"), (int&)brushBlockID);
	rectParticleBrush::serializeValue(s);
}

const resolutionTexture& blockParticleBrush::getTexture() const
{
	return *blockList[brushBlockID]->tex;
}