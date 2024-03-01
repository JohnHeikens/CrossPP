#include "renderBlockContainer.h"

bool renderBlockContainer::inBounds(cveci2& position) const
{
	return blockIDArray.inBounds(position);
}

void* renderBlockContainer::getArrayValuePointerUnsafe(cveci2& position, const arrayDataType& dataType, const chunkLoadLevel& minimalLoadLevel)
{
	switch (dataType)
	{
	case arrayDataType::blockIDType:
	{
		return (void*)&blockIDArray.getValueReferenceUnsafe(position);
	}

	case arrayDataType::blockDataType:
	{
		return (void*)&blockDataArray.getValueReferenceUnsafe(position);
	}
	case (arrayDataType)((int)arrayDataType::levelType + (int)levelID::powerLevel) :
	{
		return (void*)&powerLevelArray.getValueReferenceUnsafe(position);
	}
	}
	return nullptr;
}
