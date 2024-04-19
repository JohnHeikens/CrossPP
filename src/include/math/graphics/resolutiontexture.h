#pragma once
#include "texture.h"
#include "brush/transformbrush.h"
#include "color/color.h"

// width and height MUST be a power of 2
// https://en.wikipedia.org/wiki/Texture_mapping
struct resolutionTexture : public colorBrush
{
	//the resolution levels of this texture, from biggest [0] to smallest [size - 1]
	fastArray<texture*> scaledTextures = fastArray<texture*>();
	vec2 defaultSize = vec2();

	resolutionTexture(const texture& highestResolution, cvec2& size = cvec2(1));
	virtual ~resolutionTexture() override;

	void recalculateScaledTextures() const;

	inline rectangle2 getClientRect() const
	{
		return crectangle2(cvec2(0), defaultSize);
	}
	inline resolutionTexture(resolutionTexture& other) = delete;
	//{
	//	//static_assert(false);
	//	throw "unnecessary copy. struct way too large";
	//}

	//inline const texture& getTexture(cvec2& renderSize) const
	//{
	//	//TODO: binary trick
	//	for (size_t i = 0; ; i++)
	//	{
	//		if ((scaledTextures[i]->size.x >= renderSize.x) || (i == (scaledTextures.size - 1)))
	//		{
	//			return *scaledTextures[i];
	//		}
	//	}
	//}



	inline const texture& getMipmapTexture(cfp& renderSizeX) const
	{
		for (texture* tex : scaledTextures)
		{
			if (tex->size.x < renderSizeX)
			{
				return *tex;
			}
		}
		return *scaledTextures[scaledTextures.size - 1];
	}

	inline fp getTransformedTextureWidth(cmat3x3& transformForDefaultSize) const
	{
		return transformForDefaultSize.multSizeMatrix(cvec2(defaultSize.x, 0)).length();
	}

	//transformBrush<texture> getAntiAliasedBrush(cmat3x3& transform) const;
	const texture& mipmap(mat3x3& transform, rectangle2& textureRect) const;

	static texture getHalfResolution(const texture& doubleResolution);
	static texture getDoubleResolution(const texture& halfResolution);

	inline fp getMultiplier(cfp& renderSizeX) const
	{
		return getMipmapTexture(renderSizeX).size.x / defaultSize.x;
	}

	//scale modifier goes up if the loaded texture is bigger
	inline fp getScaleModifier() const
	{
		return scaledTextures[0]->size.x / defaultSize.x;
	}

	inline color getValue(cvec2& pos) const
	{
		return scaledTextures[0]->getValue(pos * getScaleModifier());
	}

	//the rect should be relative to the default size.
	template<typename brush0Type>
	inline void fillRectangle(rectangle2 rect, const brush0Type& b) const
	{
		cfp& scaleModifier = getScaleModifier();
		if (scaleModifier != 1) {
			rect = rect.multiplied(scaleModifier);
			//cheaper than inverse transform
			cmat3x3& inverseScaleTransform = mat3x3::scale(cvec2(1.0 / scaleModifier));
			const transformBrush<brush0Type>& brush = transformBrush<brush0Type>(inverseScaleTransform, b);
			scaledTextures[0]->fillRectangle(rect, brush);
		}
		else {
			scaledTextures[0]->fillRectangle(rect, b);
		}
		recalculateScaledTextures();
	}

	//the rect should be relative to the default size.
	template<typename brush0Type>
	inline void fillTransformedRectangle(crectangle2& brushRect, mat3x3 transform, const brush0Type& b) const
	{
		cfp& scaleModifier = getScaleModifier();
		if (scaleModifier != 1) {
			cmat3x3& scaleTransform = mat3x3::scale(cvec2(scaleModifier));
			const transformBrush<brush0Type>& brush = transformBrush<brush0Type>(scaleTransform.inverse(), b);
			transform = mat3x3::cross(scaleTransform, transform);
			scaledTextures[0]->fillTransformedRectangle(brushRect, transform, brush);
		}
		else {
			scaledTextures[0]->fillTransformedRectangle(brushRect, transform, b);
		}
		recalculateScaledTextures();
	}
};
const resolutionTexture& replaceIfMissing(const resolutionTexture* const& textureToReplace);