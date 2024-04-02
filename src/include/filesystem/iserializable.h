#pragma once
#include "serializer.h"
#include "file/endian.h"
struct ISerializable
{
	virtual bool serialize(const streamSerializer& s) = 0;
	bool serialize(const std::wstring& path, cbool& write, const std::endian& fileEndianness = std::endian::native);
	void clone(ISerializable& cloneTo);
};