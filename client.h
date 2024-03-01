#pragma once
#include "application/control/control.h"
#include "serverData.h"
#include "socketContainer.h"
#include "clientInput.h"
#include "playerData.h"
struct client : control, socketContainer {
	serverData currentServerData = serverData();
	vec2 earPosition = vec2();

	clientInput currentInput;
	playerData data;

	virtual void render(cveci2& position, const texture& renderTarget) override;
	client();
	virtual void mouseDown(cveci2& position, cvk& button) override;
	virtual void mouseUp(cveci2& position, cvk& button) override;
	virtual void scroll(cveci2& position, cint& scrollDelta) override;

	virtual void hover(cveci2& position) override;
	virtual void keyDown(cvk& keyCode) override;
	virtual void keyUp(cvk& keyCode) override;
	virtual void lostFocus() override;


	bool connectToServer(const serverData& server);
};
extern client* currentClient;
