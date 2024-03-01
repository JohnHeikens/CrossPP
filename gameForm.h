#pragma once
#include "include/application/control/form.h"
#include "include/math/rectangletn.h"
#include "include/math/vectn.h"
#include "include/application/control/control.h"
#include "include/math/graphics/texture.h"

extern int totalScrollDelta;

struct gameForm :public form
{
	gameForm();
	//eventHandler<mouseWheelEventArgs> scrollListener = eventHandler<mouseWheelEventArgs>();
	void render(cveci2& position, const texture& renderTarget) override;
	~gameForm() override;
	bool close() override;
	virtual void layout(crectanglei2& rect) override;
};

extern gameForm* mainForm;