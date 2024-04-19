#pragma once
#include "math/vectn.h"
#include "GlobalFunctions.h"
#include <type_traits> 

constexpr fsize_t bgraColorChannelCount = 4;
constexpr fsize_t rgbColorChannelCount = 3;
//bgra structure
template<typename t, fsize_t channelCount>
struct colortn : public vectn<t, channelCount>
{
	typedef t channelType;
	typedef const colortn<t, channelCount> ccolort;

	addMemberName(b, this->axis[0])
		addMemberName(g, this->axis[1])
		addMemberName(r, this->axis[2])
		addMemberName(a, this->axis[3])

		//memory alignment for constructors: 
		//(r, g, b) -> b, g, r
		//(h, s, v) -> v, s, h
		addMemberName(v, this->axis[0])
		addMemberName(s, this->axis[1])
		addMemberName(h, this->axis[2])

		static constexpr size_t byteSize = sizeof(t) * bgraColorChannelCount;

	static constexpr size_t bitSize = byteSize * ByteToBits;

	static constexpr bool isByteColor = std::is_same_v<std::remove_const_t<t>, byte>;
	static constexpr t maxValue = std::is_integral<t>::value ? (isByteColor ? bytemax : 0x100) : 1;

	static constexpr t halfMaxValue = maxValue / 2;
	static constexpr t quarterMaxValue = maxValue / 4;

	constexpr colortn(const colortn& other) = default;
    constexpr colortn& operator=(const colortn& other) = default;

	constexpr colortn(ccolort& c, const t& a) :vectn<t, channelCount>(c)
	{
		this->a() = a;
	}
	constexpr colortn(const t& a, const t& r, const t& g, const t& b)
	{
		this->a() = a;
		this->r() = r;
		this->g() = g;
		this->b() = b;
	}
	constexpr colortn(const t& r, const t& g, const t& b) : colortn(maxValue, r, g, b) {}
	constexpr colortn(const t& a, const t& grayscale) : colortn(a, grayscale, grayscale, grayscale) {}
	constexpr colortn(const t& grayscale) : colortn(maxValue, grayscale, grayscale, grayscale) {}
	constexpr colortn() //dont initialize a to maxvalue, let it be transparent by default
	{
	}

	explicit constexpr colortn(const vectn<t, channelCount>& in) : vectn<t, channelCount>(in)
	{

	}
	//for converting bgr to bgra
	//template <typename = std::enable_if_t<channelCount == 4>>
	//explicit constexpr colortn(const colortn<t, 3 + 4 - channelCount>& in);
	//for converting bgra to bgr
	//template <typename = std::enable_if_t<channelCount == 3>>
	//when channelcount = 3, then the other channelcount = 4
	explicit constexpr colortn(const vectn<t, 7 - channelCount>& in) : vectn<t, channelCount>(in) {}

	//https://stackoverflow.com/questions/41011900/equivalent-ternary-operator-for-constexpr-if
	template<typename t2>
	constexpr colortn(const colortn<t2, channelCount>& other)
	{
		for (fsize_t i = 0; i < channelCount; i++)
		{
			constexpr t2 otherMaxValue = colortn<t2, channelCount>::maxValue;
			if constexpr (maxValue == otherMaxValue)
			{
				this->axis[i] = other.axis[i];
			}
			else
			{
				//multiply to guarantee correct result
				if constexpr (std::is_floating_point<t>::value)
				{
					constexpr t multiplier = (maxValue / (t)otherMaxValue);
					this->axis[i] = (t)other.axis[i] * multiplier;
				}
				else if constexpr (std::is_floating_point<t2>::value)//t is not a floating point type, t2 is
				{
					constexpr t2 multiplier = ((t2)maxValue / otherMaxValue);
					this->axis[i] = (t)(other.axis[i] * multiplier);
				}
				else//both not a floating point, we can't make a constexpr multiplier
				{
					this->axis[i] = (t)((other.axis[i] * maxValue) / otherMaxValue);
				}
			}
		}
	}
	/*a((maxValue == colort<t2>::maxValue) ? in.a() : (in.a() * maxValue) / colort<t2>::maxValue),
	r((maxValue == colort<t2>::maxValue) ? in.r() : (in.r() * maxValue) / colort<t2>::maxValue),
	g((maxValue == colort<t2>::maxValue) ? in.g() : (in.g() * maxValue) / colort<t2>::maxValue),
	b((maxValue == colort<t2>::maxValue) ? in.b() : (in.b() * maxValue) / colort<t2>::maxValue)*/
	//{}

	template <typename t2>
	constexpr static t divideByMaxValue(const t2& toDivide)
	{
		if constexpr (maxValue == 1)
		{
			return toDivide;
		}
		else
		{
			if constexpr (std::is_integral<t2>::value)
			{
				return toDivide / maxValue;
			}
			else
			{
				constexpr fp inverseMaxValue = 1.0 / maxValue;
				return toDivide * inverseMaxValue;
			}
		}
	}

	template <typename t2>
	inline static constexpr t2 multiplyByMaxValue(const t2& toMultiply)
	{
		if constexpr (maxValue != 1)
		{
			return toMultiply * maxValue;
		}
		else
		{
			return toMultiply;
		}
	}

	template<typename t2, typename t3>
	inline static constexpr t multiplyColorChannels(const t2& channel0, const t3& channel1)
	{
		return divideByMaxValue(channel0 * channel1);
	}

	inline static constexpr colortn muliplyColors(ccolort& color0, ccolort& color1)
	{
		return ccolort(
			multiplyColorChannels(color0.r(), color1.r()),
			multiplyColorChannels(color0.g(), color1.g()),
			multiplyColorChannels(color0.b(), color1.b())
		);
	}

	//get the highest value for each channel
	inline static constexpr colortn maximizeColors(ccolort& color1, ccolort& color2)
	{
		//will be converted to << 8
		return ccolort(math::maximum(color1.r(), color2.r()),
			math::maximum(color1.g(), color2.g()),
			math::maximum(color1.b(), color2.b()));
	}

	template<size_t colorCount>
	inline static constexpr colortn interpolateColor(const colortn<fp, channelCount>(&colorsToInterpolate)[colorCount], cfp weight[colorCount])
	{
		colortn<fp, channelCount> result = colortn();
		for (size_t i = 0; i < colorCount; i++)
		{
			result.r() += (colorsToInterpolate[i].r() * weight[i]);
			result.g() += (colorsToInterpolate[i].g() * weight[i]);
			result.b() += (colorsToInterpolate[i].b() * weight[i]);
		}
		return result;
	}

	//https://en.wikipedia.org/wiki/Bilinear_interpolation
	inline static constexpr colortn interpolateColor(const colortn<fp, channelCount>(&colorsToInterpolate)[4], cvec2& weights)
	{
		cvec2& invertedWeigths = (fp)1 - weights;
		cfp weightArray[4]
		{
			invertedWeigths.x * invertedWeigths.y,
			weights.x * invertedWeigths.y,
			invertedWeigths.x * weights.y,
			weights.x * weights.y,
		};

		return interpolateColor<4>(colorsToInterpolate, weightArray);

		//return ccolort(
		//	(colorsToInterpolate[0].r() * weightArray[0]) + (colorsToInterpolate[1].r() * weightArray[1]) + (colorsToInterpolate[2].r() * weightArray[2]) + (colorsToInterpolate[3].r() * weightArray[3]),
		//	(colorsToInterpolate[0].g() * weightArray[0]) + (colorsToInterpolate[1].g() * weightArray[1]) + (colorsToInterpolate[2].g() * weightArray[2]) + (colorsToInterpolate[3].g() * weightArray[3]),
		//	(colorsToInterpolate[0].b() * weightArray[0]) + (colorsToInterpolate[1].b() * weightArray[1]) + (colorsToInterpolate[2].b() * weightArray[2]) + (colorsToInterpolate[3].b() * weightArray[3])
		//);
	}

	inline static constexpr colortn FromHex(cuint& hex)
	{
		if constexpr (maxValue != bytemax)
		{
			constexpr fp multiplier = maxValue / bytemax;
			return ccolort((hex / 0x10000) * multiplier, ((hex / 0x100) & 0xff) * multiplier, (hex & 0xff) * multiplier);
		}
		else
		{
			return ccolort((hex / 0x10000), ((hex / 0x100) & 0xff), (hex & 0xff));
		}
	}
	inline static colortn fromwstring(std::wstring str)
	{
		if (str[0] == L'#')
		{
			long l;
			if (convertToLong(str.substr(1, 6), l, 16))
			{
				return FromHex(l);
			}
		}
		throw "can't parse the color";
	}
	//returns the average of 4 colors.
	//inline static colortn Average(ccolort& c1, ccolort& c2, ccolort& c3, ccolort& c4)
	//{
	//	return ccolort(
	//		(c1.a() + c2.a() + c3.a() + c4.a()) / 4,
	//		(c1.r() + c2.r() + c3.r() + c4.r()) / 4,
	//		(c1.g() + c2.g() + c3.g() + c4.g()) / 4,
	//		(c1.b() + c2.b() + c3.b() + c4.b()) / 4
	//	);
	//}

	//returns the average of 4 colors.
	inline static colortn Average(ccolort& c1, ccolort& c2, ccolort& c3, ccolort& c4)
	{
		const auto& totalA = c1.a() + c2.a() + c3.a() + c4.a();
		if (totalA == 0)
		{
			return ccolort();
		}
		else {
			return ccolort(
				(c1.a() + c2.a() + c3.a() + c4.a()) / 4,
				(c1.r() * c1.a() + c2.r() * c2.a() + c3.r() * c3.a() + c4.r() * c4.a()) / totalA,
				(c1.g() * c1.a() + c2.g() * c2.a() + c3.g() * c3.a() + c4.g() * c4.a()) / totalA,
				(c1.b() * c1.a() + c2.b() * c2.a() + c3.b() * c3.a() + c4.b() * c4.a()) / totalA
			);
		}
	}


	//returns c1 on 0 and c2 on 1 and lerps between
	inline static constexpr colortn lerpcolor(ccolort& c1, ccolort& c2, cfp& weight)
	{
		return ccolort(
			c1.r() + (t)(weight * (fp)(c2.r() - c1.r())),
			c1.g() + (t)(weight * (fp)(c2.g() - c1.g())),
			c1.b() + (t)(weight * (fp)(c2.b() - c1.b()))
		);
	}
	//returns the 'top view' of the colors, above eachother
	inline static constexpr ccolort transition(ccolort& topcolor, ccolort& bottomcolor)
	{
		return ccolort(
			maxValue - multiplyColorChannels(maxValue - bottomcolor.a(), maxValue - topcolor.a()),
			bottomcolor.r() + multiplyColorChannels(topcolor.a(), topcolor.r() - bottomcolor.r()),
			bottomcolor.g() + multiplyColorChannels(topcolor.a(), topcolor.g() - bottomcolor.g()),
			bottomcolor.b() + multiplyColorChannels(topcolor.a(), topcolor.b() - bottomcolor.b())
		);
	}
	std::wstring getName()
	{
		throw "not made yet";
	}
};
typedef byte colorChannel;

typedef colortn<colorChannel, bgraColorChannelCount> color;
typedef const color ccolor;

typedef colortn<byte, bgraColorChannelCount> colorb;
typedef const colorb ccolorb;

typedef colortn<fp, bgraColorChannelCount> colorf;
typedef const colorf ccolorf;

namespace colorPalette
{
	static constexpr color transparent = color(0, 0);
	static constexpr color black = color(0, 0, 0);
	static constexpr color red = color(color::maxValue, 0, 0);
	static constexpr color green = color(0, color::maxValue, 0);
	static constexpr color blue = color(0, 0, color::maxValue);
	static constexpr color yellow = color(color::maxValue, color::maxValue, 0);
	static constexpr color magenta = color(color::maxValue, 0, color::maxValue);
	static constexpr color cyan = color(0, color::maxValue, color::maxValue);
	static constexpr color purple = color(color::halfMaxValue, 0, color::maxValue);
	static constexpr color white = color(color::maxValue, color::maxValue, color::maxValue);
	static constexpr color gray = color(color::halfMaxValue, color::halfMaxValue, color::halfMaxValue);
	static constexpr color orange = color(color::maxValue, color::halfMaxValue, 0);
	static constexpr color brown = color(color::halfMaxValue, color::quarterMaxValue, color::quarterMaxValue);
}


colorf rgb2hsv(const colorf& in);
colorf hsv2rgb(const colorf& in);