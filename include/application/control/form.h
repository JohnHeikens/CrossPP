#include "control.h"

#pragma once
struct form :public control 
{
	bool(*onClose);
	form();
	virtual bool close();
};