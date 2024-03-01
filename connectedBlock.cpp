#include "connectedBlock.h"
#include "blockContainer.h"
void connectedBlock::render(const renderData& targetData, blockData* const data, blockContainer* containerIn, cveci2& blockPosition) const
{
	if (identifier == blockID::kelp)
	{
		const solidColorBrush& b = solidColorBrush(waterColor);
		const auto& m = colorMixer<solidColorBrush, texture>(b, targetData.renderTarget);
		fillTransformedBrushRectangle(crectangle2(cvec2(blockPosition), cvec2(1)), targetData.worldToRenderTargetTransform, m, targetData.renderTarget);
	}

	const resolutionTexture& texToDraw = containerIn->getBlockID(blockPosition + topRelativeLocation) == identifier ? *tex : *topTexture;

	block::render(targetData, data, containerIn, blockPosition);
}