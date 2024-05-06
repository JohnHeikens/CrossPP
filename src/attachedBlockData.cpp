#include "attachedBlockData.h"
#include "idAnalysis.h"
#include "dropData.h"
#include <string>
#include "blockID.h"
#include "chunkLoadLevel.h"
#include "constants.h"
#include "math/direction.h"
#include "math/vector/vectn.h"
#include "tickableBlockContainer.h"
directionID attachedBlockData::getAttachmentDirection(tickableBlockContainer* containerIn, cveci2& position) const
{
	const blockID& block = containerIn->getBlockID(position);

	if (attachedToBottomBlock(block))
	{
		return directionID::negativeY;
	}
	handleError(std::wstring(L"block has no attachment direction"));
	return directionID();
}

bool attachedBlockData::tick(tickableBlockContainer* containerIn, cveci2& position)
{
	if (!attached(containerIn, position))
	{
		containerIn->harvestBlock(dropData(), position, chunkLoadLevel::updateLoaded);
		containerIn->setBlockID(position, blockID::air, chunkLoadLevel::updateLoaded);
	}
	return false;
}

bool attachedBlockData::attached(tickableBlockContainer* containerIn, cveci2& position) const
{
	const directionID& attachmentDirection = getAttachmentDirection(containerIn, position);
	return containerIn->canAttachTo(position + directionVectors2D[(size_t)attachmentDirection], flipDirection(attachmentDirection));
}