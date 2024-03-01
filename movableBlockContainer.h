#pragma once
#include "tickableBlockContainer.h"
#include "arrayDataType.h"
#include "blockData.h"
#include "blockID.h"
#include "chunkLoadLevel.h"
#include "collisionDataCollection.h"
#include "constants.h"
#include "include/array/arraynd.h"
#include "include/math/rectangletn.h"
#include "include/math/vectn.h"
#include "nbtSerializable.h"
#include "nbtSerializer.h"
#include "blockArrayContainer.h"

struct movableBlockContainer : tickableBlockContainer, public blockArrayContainer, nbtSerializable
{
	veci2 relativeAttachmentPosition = cveci2();
	veci2 arrayPos00Offset = veci2();//for if the array expands

	virtual void* getArrayValuePointerUnsafe(cveci2& position, const arrayDataType& dataType, const chunkLoadLevel& minimalLoadLevel) override;
	virtual bool inBounds(cveci2& position) const override;

	rectanglei2 getCurrentArrayRect() const;

	virtual collisionDataCollection getHitboxCollisionData(crectangle2& box);
	virtual bool cropRelativeLineToContainer(vec2& p0, vec2& p1) override;

	movableBlockContainer(cvect2<size_t>& size) :blockArrayContainer(size) {}
	virtual void serializeValue(nbtSerializer& s) override;
	virtual ~movableBlockContainer() override;
};