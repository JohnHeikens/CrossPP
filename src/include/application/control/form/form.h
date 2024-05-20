#include "application/control/control.h"

#pragma once
struct form :public control 
{
	bool(*onClose);
	form();
	virtual bool close();
    virtual bool wantsTextInput() const override;
};