#pragma once
#include "include/math/timemath.h"
#include "include/application/control/form.h"
struct creditsForm : form
{
	microseconds timeStartedReading = 0;

	creditsForm();
	virtual void render(cveci2& position, const texture& renderTarget) override;
	virtual void keyDown(cvk& keycode) override;
};