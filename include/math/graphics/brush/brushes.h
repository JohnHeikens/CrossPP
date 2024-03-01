#pragma once
#include "math/graphics/graphics.h"
#include "transformbrush.h"
#include "math/graphics/resolutiontexture.h"

template<typename resultingType, typename inputType>
struct solidBrush final :public brush<resultingType, inputType>
{
	resultingType value = resultingType();
	solidBrush(const resultingType& value) :value(value) {}
	inline resultingType getValue(const inputType& pos) const noexcept final
	{
		return value;
	}
};

typedef solidBrush<color, vec2> solidColorBrush;

template<typename brush0Type>
struct saturator final :public colorBrush
{
	fp addsaturation = 0.3;
	fp addvalue = 0.1;
	const brush0Type& baseBrush;
	saturator(const brush0Type& baseBrush) :baseBrush(baseBrush) {}
	inline color getValue(cvec2& pos) const final
	{
		const color original = baseBrush.getValue(pos);
		cvec3& rgbOriginal = original.toVector();
		cvec3& hsv = rgb2hsv(rgbOriginal);
		cvec3& saturated = vec3(hsv.h(), math::minimum(hsv.s + addsaturation, (fp)1.0), math::minimum(hsv.v + addvalue, (fp)1.0));
		cvec3& rgbSaturated = hsv2rgb(saturated);
		return color::fromVector(rgbSaturated);
	}
};

template<typename brush0Type, typename brush1Type>
struct colorMultiplier final : public colorBrush
{
	const brush0Type& baseBrush;
	const brush1Type& multColors;
	colorMultiplier(const brush0Type& baseBrush, const brush1Type& multColors) : baseBrush(baseBrush), multColors(multColors) {}
	inline color getValue(cvec2& pos) const final
	{
		const color& original = baseBrush.getValue(pos);
		const color& multiplyWith = multColors.getValue(pos);
		color multiplied = color::muliplyColors(original, multiplyWith);
		multiplied.a() = color::multiplyColorChannels(original.a(), multiplyWith.a());
		return multiplied;
	}
};

template<typename brush0Type, typename brush1Type>
struct colorMaximizer final : public colorBrush
{
	const brush0Type& brush0 = nullptr;
	const brush1Type& brush1 = nullptr;
	colorMaximizer(const brush0Type& brush0, const brush1Type& brush1) : brush0(brush0), brush1(brush1) {}
	virtual inline color getValue(cvec2& pos) const final
	{
		return color::maximizeColors(brush0.getValue(pos), brush1.getValue(pos));
	}
};

template<typename brush0Type, typename brush1Type>
struct alphaMask final :public colorBrush
{
	const brush0Type& alphaTex = nullptr;
	const brush1Type& baseBrush = nullptr;

	alphaMask(const brush0Type& AlphaTex, const brush1Type& baseBrush) :alphaTex(AlphaTex), baseBrush(baseBrush) {}
	inline color getValue(cvec2& pos) const final
	{
		const color c = baseBrush.getValue(pos);
		const colorChannel& alpha = alphaTex.getValue(pos).a();
		return color(c, alpha);
	}

};

template<typename brush0Type = colorBrush, typename brush1Type = colorBrush>
struct colorMixer final : public colorBrush
{
	const brush0Type& topBrush = nullptr;
	const brush1Type& bottomBrush = nullptr;

	colorMixer(const brush0Type& topBrush, const brush1Type& bottomBrush) :topBrush(topBrush), bottomBrush(bottomBrush) {}

	inline static constexpr color getColor(ccolor& topColor, ccolor& bottomColor)
	{
		//static functions
		return (topColor.a() == color::maxValue) ?
			topColor :
			((topColor.a() == 0) ? bottomColor : color::transition(topColor, bottomColor));
	}

	inline color getValue(cvec2& pos) const final
	{
		//the bottom color will be optimized away if the topcolor does not have transparency
		return getColor(topBrush.getValue(pos), bottomBrush.getValue(pos));
	}
};

template<typename brush0Type, typename resultingType = brush0Type::resultingType, typename inputType = brush0Type::inputType>
struct repeatingBrush final : public brush<resultingType, inputType>
{
	//repeats the same brush
	const brush<resultingType, inputType>& brushToRepeat;
	inputType repeatSize;
	repeatingBrush(const brush0Type& brushToRepeat, const inputType& repeatSize) :brushToRepeat(brushToRepeat), repeatSize(repeatSize) {}
	repeatingBrush(const brush0Type& textureToRepeat) :brushToRepeat(textureToRepeat), repeatSize(textureToRepeat.getClientRect().size) {}
	inline resultingType getValue(const inputType& pos) const final
	{
		if constexpr (std::is_same_v<inputType, vec2>) {
			const inputType& remainderPos = vec2(math::mod(pos.x(), repeatSize.x()), math::mod(pos.y(), repeatSize.y()));
			return brushToRepeat.getValue(remainderPos);
		}
		else {
			const inputType& remainderPos = inputType(pos.x() % repeatSize.x(), pos.y() % repeatSize.y());
			return brushToRepeat.getValue(remainderPos);
		}
	}
};

//template<typename inputType>
//struct repeatingBrush final : public brush<color, inputType>
//{
//	//repeats the same brush
//	const brush<color, inputType>& brushToRepeat = nullptr;
//	const inputType& repeatSize = inputType();
//	repeatingBrush(const brush0Type& brushToRepeat, const inputType& repeatSize) :brushToRepeat(brushToRepeat), repeatSize(repeatSize) {}
//	repeatingBrush(const brush0Type& textureToRepeat) :brushToRepeat(textureToRepeat), repeatSize(textureToRepeat.getClientRect().size) {}
//	inline color getValue(const inputType& pos) const final
//	{
//		if constexpr (std::is_same_v<inputType, vec2>) {
//			const inputType& remainderPos = vec2(math::mod(pos.x(), repeatSize.x()), math::mod(pos.y(), repeatSize.y()));
//			return brushToRepeat.getValue(remainderPos);
//		}
//		else {
//			const inputType& remainderPos = inputType(pos.x() % repeatSize.x(), pos.y() % repeatSize.y());
//			return brushToRepeat.getValue(remainderPos);
//		}
//	}
//};

template <typename colorFunction>
struct functionPointerBrush final : public colorBrush
{
	functionPointerBrush(const colorFunction&& function) :function(function) {}
	const colorFunction&& function;

	inline color getColor(cvec2& pos) const final
	{
		return function(pos);
	}
};

template<typename brush0Type>
struct grayScaleToTransparency final : colorBrush
{
	const brush0Type& grayScaleBrush;
	inline grayScaleToTransparency(const brush0Type& grayScaleBrush) :grayScaleBrush(grayScaleBrush) {}
	inline color getValue(cvec2& pos) const final
	{
		//r, g or b does not matter
		const colorChannel& alpha = grayScaleBrush.getValue(pos).r;
		return color(alpha, 0); // g->getValue(pos);//color((byte)(invert ? 0xff - alpha : alpha), transparentColor.r, transparentColor.g, transparentColor.b);
	}
};

template<typename brush0Type>
struct vignetteBrush final : colorBrush
{
	fp distanceMultiplier = 0;
	vec2 middle = vec2();
	const brush0Type& baseBrush;
	vignetteBrush(cvec2& middle, cfp& darkenAmountAtMaxDistance, cfp& maxDistance, const brush0Type& baseBrush) :distanceMultiplier((darkenAmountAtMaxDistance* color::maxValue) / math::squared(maxDistance)), middle(middle), baseBrush(baseBrush) {}
	virtual inline color getValue(cvec2& pos) const final
	{
		return color::muliplyColors(baseBrush.getValue(pos), color::maxValue - (colorChannel)((pos - middle).lengthSquared() * distanceMultiplier));
	}
};

struct bilinearInterpolator final :colorBrush
{
	const texture& baseTexture;
	inline color getValue(cvec2& pos) const final
	{
		//first interpolate x, then y
		cvect2<fsize_t> pos00 = floorVector<fsize_t>(pos);
		cvect2<fsize_t> pos11 = cvect2<fsize_t>(
			math::minimum((fsize_t)(pos00.x() + 1), baseTexture.size.x() - 1),
			math::minimum((fsize_t)(pos00.y() + 1), baseTexture.size.y() - 1)
		);

		const colorf colorsToInterpolate[]
		{
			baseTexture.getValueUnsafe(pos00),
			baseTexture.getValueUnsafe(cvect2<fsize_t>(pos11.x(), pos00.y())),
			baseTexture.getValueUnsafe(cvect2<fsize_t>(pos00.x(), pos11.y())),
			baseTexture.getValueUnsafe(pos11)
		};

		cvec2 weight = pos - cvec2(pos00);
		return color::interpolateColor(colorsToInterpolate, weight);
	}
	bilinearInterpolator(const texture& baseTexture) :baseTexture(baseTexture) {}
};

struct squareInterpolator final : public colorBrush
{
	colorf cornerColors[4]{};
	inline color getValue(cvec2& pos) const final
	{
		return colorf::interpolateColor(cornerColors, pos);
	}

	inline squareInterpolator()
	{

	}

	inline squareInterpolator(ccolor cornerColors[4])
	{
		std::copy(&cornerColors[0], &cornerColors[4], this->cornerColors);
	}
};

template<typename brush0Type, typename targetType>
inline void fillTransformedBrushRectangle(crectangle2& brushRect, cmat3x3& transform, const brush0Type& b, const targetType& renderTarget)
{
	//+0.5, * transform, -0.5
	const transformBrush<brush0Type>& brush = transformBrush<brush0Type>(transform.inverse() , b);
	renderTarget.fillTransformedRectangle(brushRect, transform, brush);
}

template<typename targetType, typename = std::enable_if_t<!std::is_same_v<targetType, resolutionTexture>>>
inline void fillTransformedBrushRectangle(rectangle2 brushRect, mat3x3 transform, const resolutionTexture& b, const targetType& renderTarget)
{
	const texture& tex = b.mipmap(transform, brushRect);
	fillTransformedBrushRectangle(brushRect, transform, tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& brushRect, cmat3x3& transform, const brush0Type& b, const targetType& renderTarget)
{
	const auto& matrixBrush = transformBrush<brush0Type>(transform.inverse(), b);
	const auto& transparencyBrush = colorMixer<transformBrush<brush0Type>, targetType>(matrixBrush, renderTarget);
	renderTarget.fillTransformedRectangle(brushRect, transform, transparencyBrush);
}

template<typename targetType, typename = std::enable_if_t<!std::is_same_v<targetType, resolutionTexture>>>
inline void fillTransparentRectangle(rectangle2 brushRect, mat3x3 transform, const resolutionTexture& b, const targetType& renderTarget)
{
	const texture& tex = b.mipmap(transform, brushRect);
	fillTransparentRectangle(brushRect, transform, tex, renderTarget);
}

template<typename brush0Type, typename brush1Type, typename targetType>
inline void multiplyRectangle(crectangle2& rect, const brush0Type& baseBrush, const brush1Type& multiplyColorsWith, const targetType& renderTarget)
{
	const auto& multiplier = colorMultiplier<brush0Type, brush1Type>(baseBrush, multiplyColorsWith);
	renderTarget.fillRectangle(rect, multiplier);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedBrushRectangle(crectangle2& brushRect, cvec2& drawOffset, const brush0Type& b, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(brushRect, crectangle2(drawOffset, brushRect.size), b, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedBrushRectangle(crectangle2& brushRect, crectangle2& rectangleToTransform, cmat3x3& transform, const brush0Type& b, const targetType& renderTarget)
{
	//from brush rect to rectangleToTransform to transformed rectangle
	fillTransformedBrushRectangle(brushRect, mat3x3::cross(transform, mat3x3::fromRectToRect(brushRect, rectangleToTransform)), b, renderTarget);
}


template<typename brush0Type, typename targetType>
inline void fillTransformedBrushRectangle(crectangle2& brushRect, crectangle2& drawRect, const brush0Type& b, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(brushRect, mat3x3::fromRectToRect(brushRect, drawRect), b, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedTexture(mat3x3 transform, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(crectangle2(tex.getClientRect()), transform, tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedTexture(crectangle2& rect, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(crectangle2(tex.getClientRect()), rect, tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedTexture(cvec2& pos, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(crectangle2(tex.getClientRect()), mat3x3::translate(pos), tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillTransformedTexture(const brush0Type& tex, const targetType& renderTarget)
{
	fillTransformedBrushRectangle(crectangle2(tex.getClientRect()), mat3x3(), tex, renderTarget);
}


template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& brushRect, crectangle2& drawRect, const brush0Type& b, const targetType& renderTarget)
{
	fillTransparentRectangle(brushRect, mat3x3::fromRectToRect(brushRect, drawRect), b, renderTarget);
}

template<typename brush0Type, typename targetType>
inline static void fillTransparentTexture(crectangle2& drawRect, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransparentRectangle(crectangle2(tex.getClientRect()), drawRect, tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline static void fillTransparentTexture(cmat3x3& transform, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransparentRectangle(crectangle2(tex.getClientRect()), transform, tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline static void fillTransparentTexture(crectangle2& textureRect, cvec2& drawPosition, const brush0Type& tex, const targetType& renderTarget)
{
	fillTransparentRectangle(crectangle2(textureRect), crectangle2(drawPosition, textureRect.size), tex, renderTarget);
}

template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& brushRect, cint& rotation, crectangle2& drawRect, const brush0Type& b, const targetType& renderTarget)
{
	fillTransparentRectangle(brushRect, mat3x3::fromRectToRotatedRect(brushRect, rotation, drawRect), b, renderTarget);
}
template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& drawRect, const brush0Type& b, const targetType& renderTarget)
{
	renderTarget.fillRectangle(drawRect, colorMixer<brush0Type, targetType>(b, renderTarget));
}

template<typename targetType>
inline static void fillTransparentRectangle(crectangle2& drawRect, const color& c, const targetType& renderTarget)
{
	fillTransparentRectangle(drawRect, solidColorBrush(c), renderTarget);
}
template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& brushRect, crectanglei2& drawRect, const brush0Type& b, const targetType& renderTarget)
{
	fillTransparentRectangle(brushRect, crectangle2(drawRect), b, renderTarget);
}
template<typename brush0Type, typename targetType>
inline static void fillTransparentRectangle(crectangle2& brushRect, crectangle2& rectangleToTransform, cmat3x3& transform, const brush0Type& b, const targetType& renderTarget)
{
	//from brush rect to rectangleToTransform to transformed rectangle
	fillTransparentRectangle(brushRect, mat3x3::cross(transform, mat3x3::fromRectToRect(brushRect, rectangleToTransform)), b, renderTarget);
}

template<typename brush0Type, typename targetType>
inline void fillRepeatingRectangle(crectangle2& rect, const brush0Type& b, cvec2& repeatSize, const targetType& renderTarget)
{
	const auto& repeater = repeatingBrush<brush0Type>(b, repeatSize);
	renderTarget.fillRectangle(ceilRectangle(rect), repeater);
}

template<typename brush0Type, typename targetType>
inline void fillRepeatingTexture(crectangle2& rect, const brush0Type& tex, const targetType& renderTarget)
{
	fillRepeatingRectangle(rect, tex, tex.size, renderTarget);
}

/// <summary>
/// CAUTION! this will multiply the RECTANGLE, not the TEXTURE!
/// </summary>
/// <typeparam name="brush0Type"></typeparam>
/// <typeparam name="targetType"></typeparam>
/// <param name="rect"></param>
/// <param name="transform"></param>
/// <param name="tex"></param>
/// <param name="renderTarget"></param>
template<typename brush0Type, typename targetType>
inline void fillUnTransformedRepeatingTexture(crectangle2& rect, cmat3x3& transform, const brush0Type& b, cvec2& repeatSize, const targetType& renderTarget)
{
	const auto& repeater = repeatingBrush<brush0Type>(b, repeatSize);
	renderTarget.fillTransformedRectangle(rect, transform, repeater);
}

template<typename brush0Type, typename targetType>
inline void multiplyRectangle(crectangle2& rect, const brush0Type& multiplyColorsWith, const targetType& renderTarget)
{
	multiplyRectangle(rect, renderTarget, multiplyColorsWith, renderTarget);
}

template<typename targetType>
inline void multiplyRectangle(crectangle2& rect, const color& multiplyColorsWith, const targetType& renderTarget)
{
	const solidColorBrush b = solidColorBrush(multiplyColorsWith);
	multiplyRectangle(rect, b, renderTarget);
}

namespace brushes
{
	extern solidColorBrush black;
	extern solidColorBrush white;
	extern solidColorBrush red;
	extern solidColorBrush green;
	extern solidColorBrush blue;
	extern solidColorBrush cyan;
	extern solidColorBrush orange;
	extern solidColorBrush transparent;
}