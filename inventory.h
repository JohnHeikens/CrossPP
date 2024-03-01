#pragma once
#include "include/math/graphics/resolutiontexture.h"
#include "slotContainer.h"
#include "include/interface/idestructable.h"
#include "uiSlotContainer.h"
constexpr rectanglei2 defaultUIRectangle = crectanglei2(0, 90, 176, 166);
struct inventory : slotContainer, IDestructable
{
	human* linkedPlayer = nullptr;
	resolutionTexture* uiTexture = nullptr;
	rectanglei2 uiTexturePartToDraw = defaultUIRectangle;//default

	std::vector<uiSlotContainer*> containers = std::vector<uiSlotContainer*>();
	uiSlotContainer* hotbarSlots = new uiSlotContainer(cveci2(0x8, 0x8), nullptr);
	uiSlotContainer* inventorySlots = new uiSlotContainer(cveci2(0x8, 30), nullptr);
	virtual void clickedOnItem(cvk& button, itemStack& stackHolding, uiSlotContainer* selectedSlotContainer, veci2 selectedSlot);
	inline void drawExtraData(crectanglei2& textureRect, cveci2& offset, cmat3x3& transform, const texture& renderTarget) const {
		fillTransparentRectangle(crectangle2(textureRect), mat3x3::cross(transform, mat3x3::translate(cvec2(offset - textureRect.pos0))), *uiTexture, renderTarget);

	}
	virtual void drawExtraData(cmat3x3& transform, const texture& renderTarget);
	virtual ~inventory() override;
	virtual void mouseDown(cveci2& pixelPosition, cvk& button, itemStack& stackHolding);
	virtual void drawToolTips(cveci2& mouseTexturePosition, cveci2& mousePositionPixels, const texture& renderTarget);
	virtual bool canAddStack(uiSlotContainer* containerToAddTo, itemStack& s);

	uiSlotContainer* getSlotContainer(cveci2& mousePositionPixels, veci2& slotPosition);
};