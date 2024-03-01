#pragma once
#include "include/interface/idestructable.h"
#include "include/interface/inamable.h"
#include "include/filesystem/iserializable.h"
#include "nbtDataTag.h"

struct nbtData :INamable, ISerializable, IDestructable
{
	nbtDataTag dataTag = (nbtDataTag)0;
	virtual bool serialize(const streamSerializer& s) override = 0;
	virtual void serialize(cbool& write, std::wstring& s);
	nbtData(const std::wstring& name, const nbtDataTag& dataTag) :INamable(name), dataTag(dataTag) {}
	virtual bool compare(const nbtData& other)  const = 0;
};

nbtData* createNBTData(const std::wstring& text);