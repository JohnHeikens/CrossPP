#include "graphics.h"

graphicsObject::~graphicsObject()
{
	//don't delete anything
	baseArray = nullptr;
}

graphicsObject::graphicsObject(cveci2& size, color* const& baseArray) :texture(size, baseArray)
{
}

graphicsObject* graphicsObject::fromTexture(const texture& tex)
{
	return new graphicsObject(tex.size, tex.baseArray);
}

graphicsObject::graphicsObject(const graphicsObject& other) : texture(other.size, other.baseArray)
{
	//don't copy
}