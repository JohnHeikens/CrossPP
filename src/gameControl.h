#pragma once
#include "application/control/form.h"
#include "structureBlockUI.h"
#include "jigsawUI.h"
#include "inventoryForm.h"
#include "settingsForm.h"
#include "videoSettingsForm.h"
#include "soundSettingsForm.h"
#include "musicCollection.h"
#include "gameRenderData.h"
#include "clientInput.h"
#include "creditsForm.h"
#include "soundPacket.h"
#include <sfml/Graphics/RenderTexture.hpp>

constexpr fp defaultGuiTextureSize = (fp)0x200;
struct playerSocket;
//the game will be rendered on a texture using this class
struct gameControl : form, clientInput
{
	gameControl(playerSocket& socket);
	//virtual void render(cveci2& position, const texture& renderTarget) override;
	void render(cveci2& position, const texture& renderTarget);
	virtual void processInput();
	virtual void layout(crectanglei2& newRect) override;
	virtual void focus() override;
	virtual void lostFocus() override;

	settingsForm* options = new settingsForm();
	videoSettingsForm* videoOptions = new videoSettingsForm();
	soundSettingsForm* soundOptions = new soundSettingsForm();
	structureBlockUI* structureBlockOptions = new structureBlockUI();
	jigsawUI* jigsawOptions = new jigsawUI();
	textBox* commandLineTextbox = new textBox();
	inventoryForm* inventoryUI = new inventoryForm();
	creditsForm* currentCredits = new creditsForm();

	bool startCredits = false;

	//connections
	playerSocket& socket;
	human* player;//for less code

	//rendering
	vec2 cameraPosition = vec2();
	fp hudScale;

	fp visibleRangeXWalk = 0xa;
	vec2 visibleRange = vec2(visibleRangeXWalk);

	veci2 unFocusedMousePosition = veci2();
	vec2 currentMousePositionWorld = vec2();
	//veci2 mousePositionPixels = veci2();

	//veci2 newMousePositionPixels = veci2();
	//controls respond faster, so we have another input for controls

	clientInput mostRecentInput = clientInput();

	bool clickedFocused[(byte)sf::Mouse::ButtonCount];

	//creditsForm* currentCredits = nullptr;

	mat3x3 worldToRenderTargetTransform = mat3x3();

	std::vector<soundPacket> dataToSend = std::vector<soundPacket>();

	void renderGame(crectanglei2& rect, const texture& renderTarget, cbool& renderHUD);
	gameRenderData getRenderData(const texture& renderTarget, cfp& secondsOffset = 0);
	mat3x3 getWorldToScreenTransform(cvec2& middleWorldPosition, cfp& pixelsPerBlock);
};

extern std::mt19937 currentRandom;

extern std::shared_ptr<musicCollection> currentlyPlayingCollection;
void renderIcons(const std::vector<fp>& values, const std::vector<rectangle2>& iconFullTextureRects, const std::vector<rectangle2>& iconHalfTextureRects, crectangle2& firstIconRect, cfp& xOffset, const texture& renderTarget);
void renderOptionsBackGround(crectanglei2& rect, const texture& renderTarget);