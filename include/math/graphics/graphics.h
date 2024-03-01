#pragma once
#include "texture.h"

struct graphicsObject:public texture
{
	//begin
	graphicsObject(cveci2& size = cveci2(), color* const& baseArray = nullptr);
	static graphicsObject* fromTexture(const texture& tex);	

	graphicsObject(const graphicsObject& other);

	inline graphicsObject& operator=(graphicsObject copy)
	{
		this->swap(copy);
		return *this;
	}

	virtual ~graphicsObject() override;
};

