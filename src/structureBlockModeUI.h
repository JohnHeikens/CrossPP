#pragma once
#include "application/control/form.h"
struct structureBlockModeUI : form
{
	virtual void render(cveci2& position, const texture& renderTarget) override;
};