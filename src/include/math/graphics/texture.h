#pragma once
#include "brush/brush.h"
#include "array/arraynd.h"

struct texture : public virtual colorBrushSizeT, public array2d<color>
{
public:
	texture(cvect2<fsize_t> &size, cbool &initializeToDefault = true) : array2d<color>(size, initializeToDefault) {}
	texture(cvect2<fsize_t> &size, color *const &baseArray) : array2d<color>(size, baseArray) {}

	texture(const stdPath& path, cbool &flip);
	// the colors wont be deleted when the texture is deleted.
	void switchChannels(colorb *byteArray, cint &channelIndex1, cint &channelIndex2) const;
	void Flip() const;
	bool Save(const stdPath& path) const;

	inline color getValue(cvect2<fsize_t> &pos) const
	{
		if constexpr (isDebugging)
		{
			if (!inBounds(pos))
			{
				return colorPalette::magenta;
			}
			// if (pos.x >= size.x)
			//{
			//	return colorPalette::yellow;
			// }
			// else if (pos.y >= size.y)
			//{
			//	return colorPalette::blue;
			// }
			// if constexpr (std::is_signed_v<fsize_t>)
			//{
			//	if (pos.x < 0)
			//	{
			//		return colorPalette::orange;
			//	}
			//	else if (pos.y < 0)
			//	{
			//		return colorPalette::green;
			//	}
			// }
		}
		return getValueUnsafe(pos);
	}

	static void Barycentric(cvec2 &p, cvec2 &a, cvec2 &b, cvec2 &c, fp &u, fp &v, fp &w);

	template <typename T>
	inline static fp interpolate(const T &u1, const T &v1, const T &w1, cvec2 p)
	{
		return 1 - p.x - p.y * u1 + p.x * v1 + p.y * w1;
	}

	template <typename T>
	inline static void getcoordfunction(const T &u1, const T &v1, const T &w1, const mat3x3 matrix, T &begin, T &xplus, T &yplus)
	{
		vec2 coord00 = matrix.getstart(); // x 0 y 0
		begin = (1 - coord00.x - coord00.y) * u1 + coord00.x * v1 + coord00.y * w1;
		vec2 coordxplus = matrix.getStep<2>(axisID::x);
		// the ones cancel eachother out
		xplus = (-coordxplus.x - coordxplus.y) * u1 + coordxplus.x * v1 + coordxplus.y * w1;
		vec2 coordyplus = matrix.getStep<2>(axisID::y);
		yplus = (-coordyplus.x - coordyplus.y) * u1 + coordyplus.x * v1 + coordyplus.y * w1;
	}

	void Fade(cfp &weight, const color &fadeto) const;

	template <typename brush0Type>
	inline void visualizeFormula(crectangle2& screenRect, crectangle2& spaceRect, fp(*func)(cfp& x), const brush0Type& b)
	{
		mat3x3 spaceToScreen = mat3x3::fromRectToRect(spaceRect, screenRect);
		fp dotDistance = 0.01;
		for (fp spaceX = math::ceil(spaceRect.pos0.x); spaceX < spaceRect.pos1().x; spaceX += dotDistance)
		{
			cvec2 pos = spaceToScreen.multPointMatrix(vec2(spaceX, func(spaceX)));
			fillEllipseCentered(pos, vec2(4), b);
		}
	}
	virtual ~texture() override
	{
	}
};

veci2 getImageSize(const stdPath& path);
