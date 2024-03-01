#include "nbtSerializer.h"
#include <string>
#include <vector>
#include "constants.h"
#include "nbtCompound.h"
#include "nbtData.h"
#include "nbtDataTag.h"
#include "idConverter.h"
nbtCompound* nbtSerializer::getCurrentChildCompound() const
{
	return recursiveChildCompounds.size() ? recursiveChildCompounds[recursiveChildCompounds.size() - 1] : compound;
}

bool nbtSerializer::push(const nbtData* child)
{
	recursiveChildCompounds.push_back((nbtCompound*)child);
	nextListIndexes.push_back(0);
	return true;
}

void nbtSerializer::pop()
{
	if (recursiveChildCompounds.size())
	{
		recursiveChildCompounds.pop_back();
	}
	else
	{
		handleError(std::wstring(L"nothing to pop"));
	}
}

const std::vector<nbtData*>& nbtSerializer::getChildren() const
{
	return getCurrentChildCompound()->children;
}

bool nbtSerializer::isArrayTag(const nbtDataTag& dataTag)
{
	return dataTag == nbtDataTag::tagSignedInt8Array || dataTag == nbtDataTag::tagSignedInt32Array || dataTag == nbtDataTag::tagSignedInt64Array;
}
nbtSerializer::nbtSerializer(nbtCompound* compound, cbool& write, cbool& convert) :iSerializer(write), compound(compound), converter(converter)
{
	if (convert)
	{
		int version = currentFileVersionID;
		serializeValue(std::wstring(L"version"), version);
		converter = getConverter(version);
	}
	else
	{
		converter = nullptr;
	}
}