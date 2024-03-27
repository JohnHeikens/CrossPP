#pragma once
#include "direction.h"
#include "vectn.h"
#include "math/graphics/alignment.h"
#include "random/random.h"

template <typename t, fsize_t axisCount>
struct rectIteratortn;

template <typename t, fsize_t axisCount>
struct baseRect
{
	union
	{
		struct
		{
			vectn<t, axisCount> pos0;
			vectn<t, axisCount> size;
		};
		struct
		{
			t x;
			t y;
			t w;
			t h;
		};
	};
#define rectConstructor(axisCount)                                                                     \
	constexpr baseRect(vectn<t, axisCount> pos0, vectn<t, axisCount> size) : pos0(pos0), size(size) {} \
	constexpr ~baseRect()                                                                              \
	{                                                                                                  \
		pos0.~vectn<t, axisCount>();                                                                   \
		size.~vectn<t, axisCount>();                                                                   \
	}
	rectConstructor(axisCount) constexpr baseRect(const t &x, const t &y, const t &w, const t &h)
	{
		pos0 = vect2<t>(x, y);
		size = vect2<t>(w, h);
	}
};
template <typename t>
struct baseRect<t, 0>
{
	union
	{
		struct
		{
			vectn<t, 1> pos0;
			vectn<t, 1> size;
		};
		struct
		{
		};
	};
	rectConstructor(0)
	// constexpr baseRect() = default;
	// constexpr baseRect() : pos0(vectn<t, axisCount>(), size(vectn<t, axisCount>())) {}
};
template <typename t>
struct baseRect<t, 1>
{
	union
	{
		struct
		{
			vectn<t, 1> pos0;
			vectn<t, 1> size;
		};
		struct
		{
			t x;
			t w;
		};
	};
	rectConstructor(1)
	// constexpr baseRect() = default;
	// constexpr baseRect() : pos0(vectn<t, axisCount>(), size(vectn<t, axisCount>())) {}
};

// #cannot make an union out of this, because the union would need to vary in size depending on the axisCount parameter
template <typename t, fsize_t axisCount>
struct rectangletn : baseRect<t, axisCount>
{
	using baseRect<t, axisCount>::pos0;
	using baseRect<t, axisCount>::size;

	addMemberName(getX, pos0.getX())
		addMemberName(getY, pos0.getY())
			addMemberName(getW, size.getX())
				addMemberName(getH, size.getY())
		//
		using baseRect<t, axisCount>::baseRect;
	constexpr rectangletn() : baseRect<t, axisCount>(vectn<t, axisCount>(), vectn<t, axisCount>()) {}
	// constexpr rectangletn(cvectn<t, axisCount> &pos0, cvectn<t, axisCount> &size)
	//{
	//	this->pos0 = pos0;
	//	this->size = size;
	// }
	constexpr rectangletn(cvectn<t, axisCount> &size) : baseRect<t, axisCount>(vectn<t, axisCount>(), size) {}

	template <typename t2>
	explicit constexpr rectangletn(const rectangletn<t2, axisCount> &in) : baseRect<t, axisCount>((vectn<t, axisCount>)in.pos0, (vectn<t, axisCount>)in.size) {}

	constexpr rectIteratortn<t, axisCount> begin() const;
	constexpr rectIteratortn<t, axisCount> end() const;

	constexpr vectn<t, axisCount> pos10() const
	{
		return vec2(pos0.x + size.x, pos0.y);
	}
	constexpr vectn<t, axisCount> pos01() const
	{
		return vec2(pos0.x, pos0.y + size.y);
	}
	constexpr vectn<t, axisCount> pos1() const
	{
		return pos0 + size;
	}
	constexpr bool contains(const vectn<t, axisCount> &pos) const
	{
		for (fsize_t i = 0; i < axisCount; i++)
		{
			if ((pos[i] < pos0[i]) || (pos[i] >= (pos0[i] + size[i])))
			{
				return false;
			}
		}

		return true;
	}

	constexpr void expand(const t &border)
	{
		pos0 -= border;
		size += border * 2;
	}

	constexpr void expandToContain(const rectangletn &rect)
	{
		const vectn<t, axisCount> &oldPos11 = pos1();
		const vectn<t, axisCount> &rectPos11 = rect.pos1();
		for (int i = 0; i < axisCount; i++)
		{
			if (rect.pos0[i] < pos0[i])
			{
				size[i] += pos0[i] - rect.pos0[i];
				pos0[i] = rect.pos0[i];
			}
			else if (rectPos11[i] > oldPos11[i])
			{
				size[i] += rectPos11[i] - oldPos11[i];
			}
		}
	}

	// the rectangle will not exceed the borders; else it will return false.
	constexpr bool cropClientRect(rectangletn &toCrop) const
	{
		vectn<t, axisCount> toCropPos1 = toCrop.pos1();
		vectn<t, axisCount> borderPos1 = pos1();

		for (int i = 0; i < axisCount; i++)
		{
			if (toCrop.pos0[i] < pos0[i])
			{
				if (toCropPos1[i] < pos0[i])
				{
					return false;
				}
				toCrop.size[i] += (toCrop.pos0[i] - pos0[i]);
				toCrop.pos0[i] = pos0[i];
				// pos1 is not modified
			}

			if (toCropPos1[i] > borderPos1[i])
			{
				if (toCrop.pos0[i] > borderPos1[i])
				{
					return false;
				}
				toCrop.size[i] = borderPos1[i] - toCrop.pos0[i];
			}
		}
		return true;
	}
	constexpr rectangletn cropClientRectUnsafe(rectangletn toCrop) const
	{

		vectn<t, axisCount> toCropPos1 = toCrop.pos1();
		vectn<t, axisCount> borderPos1 = pos1();

		for (int i = 0; i < axisCount; i++)
		{
			if (toCrop.pos0[i] < pos0[i])
			{
				toCrop.size[i] += (toCrop.pos0[i] - pos0[i]);
				toCrop.pos0[i] = pos0[i];
				// pos1 is not modified
			}

			if (toCropPos1[i] > borderPos1[i])
			{
				toCrop.size[i] = borderPos1[i] - toCrop.pos0[i];
			}
		}
		// safety measures are not in place. instead of returning a boolean, we return the result
		return toCrop;
	}

	constexpr static t getCollisionArea(rectangletn rect0, const rectangletn &rect1)
	{
		if (rect1.cropClientRect(rect0))
		{
			return rect0.size.volume();
		}
		else
		{
			return 0;
		}
	}

	constexpr rectangletn expanded(const t &border) const
	{
		rectangletn r = *this;
		r.expand(border);
		return r;
	}
	constexpr rectangletn touchingBorder(const rectangletn &borders, const directionID &direction) const
	{
		rectangletn r = *this;
		caxisID &axis = getAxis(direction);
		if (isPositive(direction))
		{
			r.pos0[axis] = borders.pos0[axis] + borders.size[axis];
		}
		else
		{
			r.pos0[axis] = borders.pos0[axis] - size[axis];
		}
		return r;
	}
	constexpr vectn<t, axisCount> rectPos0Centered(const vectn<t, axisCount> &innerRectSize) const
	{
		vectn<t, axisCount> result = vectn<t, axisCount>();

		for (fsize_t i = 0; i < axisCount; i++)
		{
			result[i] = pos0[i] + ((size[i] - innerRectSize[i]) / 2);
		}
		return result;
	}
	constexpr rectangletn<t, axisCount> rectCentered(const vectn<t, axisCount> &innerRectSize) const
	{
		return rectangletn<t, axisCount>(rectPos0Centered(innerRectSize), innerRectSize);
	}
	constexpr rectangletn<t, axisCount> rectCenteredAround(const vectn<t, axisCount> &newCenterPosition) const
	{
		return rectangletn<t, axisCount>(newCenterPosition + (size / (t)2), size);
	}
	constexpr void moveToCenter(const rectangletn &outerRect)
	{
		pos0 = outerRect.rectPos0Centered(size);
	}
	constexpr rectangletn switchAxes()
	{
		return rectangletn(pos0.switchAxes(), size.switchAxes());
	}
	constexpr vectn<t, axisCount> getCenter() const
	{
		return pos0 + cvec2(size) * 0.5;
	}

	inline static constexpr rectangletn fromOppositeCorners(const vectn<t, axisCount> &corner0, const vectn<t, axisCount> &corner1)
	{
		vectn<t, axisCount> pos0 = vectn<t, axisCount>();
		vectn<t, axisCount> pos1 = vectn<t, axisCount>();

		for (size_t i = 0; i < axisCount; i++)
		{
			pos0[i] = math::minimum(corner0[i], corner1[i]);
			pos1[i] = math::maximum(corner0[i], corner1[i]);
		}
		return rectangletn(pos0, pos1 - pos0);
	}

	constexpr t getAlignedY(const t rectangleHeight, const verticalAlignment &alignment)
	{
		using baseRect<t, axisCount>::y;
		using baseRect<t, axisCount>::h;
		switch (alignment)
		{
		case verticalAlignment::bottom:
			return y;
		case verticalAlignment::middle:
			return y + (h + rectangleHeight) / 2;
		case verticalAlignment::top:
			return y + h - rectangleHeight;
		}
	}
	constexpr t getAlignedX(const t rectangleWidth, const horizontalAlignment &alignment)
	{
		using baseRect<t, axisCount>::x;
		using baseRect<t, axisCount>::w;

		switch (alignment)
		{
		case horizontalAlignment::left:
			return x;
		case horizontalAlignment::middle:
			return x + (w + rectangleWidth) / 2;
		case horizontalAlignment::right:
			return x + w - rectangleWidth;
		}
	}
	constexpr rectangletn rotatedInRectangle(const rectangletn &outerRect, const directionID &direction) const
	{
		rectangletn rotatedRectangle = *this;
		cbool positive = isPositive(direction);
		caxisID &axis = getAxis(direction);

		if (!positive)
		{
			rotatedRectangle.y = (outerRect.y + outerRect.h - rotatedRectangle.h) - (rotatedRectangle.y - outerRect.y);
		}

		if (axis == axisID::x)
		{
			return rotatedRectangle.switchAxes();
		}
		return rotatedRectangle;
	}

	inline static constexpr rectangletn unitSquare() { return rectangletn(0, 0, 1, 1); }

	constexpr bool operator==(const rectangletn &other) const
	{
		return (pos0 == other.pos0) && (size == other.size);
	}
	constexpr bool operator!=(const rectangletn &other) const
	{
		return !operator==(other);
	}

	template <typename t2>
	constexpr rectangletn multiplied(const t2 &multiplier) const
	{
		return rectangletn(pos0 * multiplier, size * multiplier);
	}
};

addTemplateTypes(rectangle)

	// DONT USE THIS TO CONVERT TO PIXEL RECTANGLES; USE CEILRECT
	template <typename outputType = int, typename inputType, fsize_t axisCount>
	inline rectangletn<outputType, axisCount> floorRectangle(crectangletn<inputType, axisCount> &rect)
{
	const auto &pos0 = floorVector<outputType>(rect.pos0);

	const auto &pos1 = floorVector<outputType>(rect.pos0 + rect.size);

	return crectangletn<outputType, axisCount>(pos0, pos1 - pos0);
}
template <typename outputType = int, typename inputType, fsize_t axisCount>
inline rectangletn<outputType, axisCount> ceilRectangle(crectangletn<inputType, axisCount> &rect)
{
	const auto &pos0 = ceilVector<outputType>(rect.pos0);

	const auto &pos1 = ceilVector<outputType>(rect.pos0 + rect.size);

	return crectangletn<outputType, axisCount>(pos0, pos1 - pos0);
}

inline vec2 getrandomPosition(std::mt19937 &generator, crectangle2 &rect, cvec2 &innerRectangleSize)
{
	return vec2(rect.x + randFp(generator, rect.w - innerRectangleSize.x), rect.y + randFp(generator, rect.h - innerRectangleSize.y));
}

constexpr rectangle2 getAbsoluteRect(crectangle2 &outerRect, crectangle2 &relativeRect)
{
	return rectangle2(
		outerRect.x + relativeRect.x * outerRect.w,
		outerRect.y + relativeRect.y * outerRect.h,
		relativeRect.w * outerRect.w,
		relativeRect.h * outerRect.h);
}
constexpr vec2 getAbsolutePosition(crectangle2 &rect, cvec2 &relativePosition)
{
	return rect.pos0 + relativePosition * rect.size;
}
constexpr vec2 getRelativePosition(crectangle2 &rect, cvec2 &absolutePosition)
{
	return (absolutePosition - rect.pos0) / rect.size;
}
inline vec2 getrandomPosition(std::mt19937 &generator, crectangle2 &rect)
{
	return vec2(rect.x + randFp(generator, rect.w), rect.y + randFp(generator, rect.h));
}

// copies a piece out of an old array into a new array
template <typename t>
inline t *cut(t *const &arr, crectanglet1<size_t> &range)
{
	t *newArray = new t[range.w];
	std::copy(arr + range.x, arr + range.pos1().x, newArray);
	return newArray;
}

// An iterator over a vector of vectors.

// https://stackoverflow.com/questions/1784573/iterator-for-2d-vector

template <typename t, fsize_t axisCount>
struct rectIteratortn
{
public:
	vectn<t, axisCount> pos = vectn<t, axisCount>();
	rectangletn<t, axisCount> rect = rectangletn<t, axisCount>();

	constexpr rectIteratortn() = default;

	constexpr rectIteratortn(crectangletn<t, axisCount> &rect, cvectn<t, axisCount> &pos) : rect(rect), pos(pos) {}

	// ++prefix operator
	constexpr rectIteratortn &operator++()
	{
		for (size_t i = 0; i < axisCount; i++)
		{
			if ((pos[i] + 1) < (rect.pos0[i] + rect.size[i]) || (i == (axisCount - 1)))
			{
				++pos[i];
				return *this;
			}
			else
			{
				pos[i] = rect.pos0[i];
			}
		}
		throw "";
	}
	// --prefix operator
	constexpr rectIteratortn &operator--()
	{
		for (size_t i = 0; i < axisCount; i++)
		{
			// If we haven't reached the end of this sub-vector.
			if ((pos[i] > rect.pos0[i]) || (i == (axisCount - 1)))
			{
				// Go to the next element.
				--pos[i];
				return *this;
			}
			else
			{
				pos[i] = rect.pos0[i] + (rect.size[i] - 1);
			}
		}
	}

	// postfix++ operator
	constexpr rectIteratortn operator++(int)
	{
		t retval = *this;
		++(*this);
		return retval;
	}
	// postfix-- operator
	constexpr rectIteratortn operator--(int)
	{
		t retval = *this;
		--(*this);
		return retval;
	}
	constexpr bool operator==(const rectIteratortn &other) const
	{
		return (other.rect == rect) && (other.pos == pos);
	}
	constexpr bool operator!=(const rectIteratortn &other) const
	{
		return !operator==(other);
	}
	constexpr const vectn<t, axisCount> &operator*() const
	{
		if constexpr (isDebugging)
		{
			if (pos[axisCount - 1] == rect.pos1()[axisCount - 1])
			{
				throw "out of rectangle";
			}
		}
		return pos;
	}
};

template <typename t, fsize_t axisCount>
constexpr rectIteratortn<t, axisCount> rectangletn<t, axisCount>::begin() const
{
	return rectIteratortn<t, axisCount>(*this, pos0);
}

template <typename t, fsize_t axisCount>
constexpr rectIteratortn<t, axisCount> rectangletn<t, axisCount>::end() const
{
	vectn<t, axisCount> endPos = pos0;

	endPos[axisCount - 1] += size[axisCount - 1];

	return rectIteratortn<t, axisCount>(*this, endPos);
}