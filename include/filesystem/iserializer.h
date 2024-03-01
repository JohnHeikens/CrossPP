#pragma once
#include "GlobalFunctions.h"
struct iSerializer
{
	//true if the serializer is writing, false if the serializer is reading
	bool write;
	iSerializer(cbool& write) :write(write) {}
};