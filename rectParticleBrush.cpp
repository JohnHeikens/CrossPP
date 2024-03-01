#include "rectParticleBrush.h"
void rectParticleBrush::serializeValue(nbtSerializer& s)
{
	s.serializeValue(std::wstring(L"brush rect"), brushRect);
}

void rectParticleBrush::render(crectangle2& rect, const renderData& targetData) const
{
	cmat3x3& transForm = mat3x3::combine({ mat3x3::fromRectToRect(brushRect,rect),targetData.worldToRenderTargetTransform });
	fillTransparentRectangle(brushRect, transForm, getTexture(), targetData.renderTarget);
}