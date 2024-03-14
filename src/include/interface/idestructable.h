#pragma once
#include "GlobalFunctions.h"
//not an interface, because those don't have a compiler generated copy operator
struct IDestructable
{
	//this makes it so base classes get deleted too.
	virtual ~IDestructable();
};