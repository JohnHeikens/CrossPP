#pragma once
#include "GlobalFunctions.h"
#include "axis.h"
//#include "array/zipiterator.h"
#include "optimization/optimization.h"
#include "direction.h"
#include "mathfunctions.h"
#include <ranges>

template<typename t, fsize_t n>
//alignment is already a power of two
struct vectn //alignas(((n == 3 || n == 4) && (alignof(t) <= 0x4)) ? (alignof(t) * 0x4) : math::getNextPowerOf2Multiplied(sizeof(t) * n)) vectn
{
	typedef const vectn cvecn;
	static constexpr fsize_t axisCount = n;
	typedef t axisType;
	t axis[math::maximum(axisCount, (fsize_t)1)]{};

	constexpr const t* begin() const noexcept
	{
		return axis;
	}

	constexpr const t* end() const noexcept
	{
		return axis + axisCount;
	}

	constexpr t* begin() noexcept
	{
		return axis;
	}

	constexpr t* end() noexcept
	{
		return axis + axisCount;
	}

	constexpr vectn() noexcept
	{
	}

	//fill with this value
	constexpr vectn(const t& value)
	{
		//references don't work in constexpr functions apparently

		//both of these don't initialize them really.
		//for (auto it = this->begin(); it != this->end(); it++)
		//{
		//	const auto& v = *it;
		//	v = value;
		//}
		//for (auto& val : (*this))
		//{
		//	val = value;
		//}

		for (auto it = this->begin(); it != this->end(); it++)
		{
			*it = value;
		}
	}

	constexpr vectn(const t& x, const t& y)
	{
		static_assert(axisCount >= 2, "too few axes for this constructor");
		(*this)[0] = x;
		(*this)[1] = y;
	}

	constexpr vectn(const t& x, const t& y, const t& z) : vectn(x, y)
	{
		static_assert(axisCount >= 3, "too few axes for this constructor");
		axis[2] = z;
	}

	constexpr vectn(const t& x, const t& y, const t& z, const t& w) : vectn(x, y, z)
	{
		static_assert(axisCount >= 4, "too few axes for this constructor");
		axis[3] = w;
	}

	constexpr vectn(std::array<t, axisCount> values)
	{
		for (fsize_t i = 0; i < values.size(); i++)
		{
			axis[i] = values[i];
		}
	}

	constexpr vectn switchAxes() const
	{
		vectn result = vectn(axis[1], axis[0]);
		if constexpr (axisCount > 2)
		{
			for (fsize_t i = 2; i < axisCount; i++)
			{
				result.axis[i] = axis[i];
			}
		}
		return result;
	}

	template<typename t2, fsize_t axisCount2>
	constexpr vectn(const vectn<t2, axisCount2>& in)
	{
		constexpr fsize_t minimum = math::minimum(axisCount, axisCount2);

		for (fsize_t i = 0; i < minimum; i++)
		{
			axis[i] = (t)in.axis[i];
		}
	}

	constexpr vectn(const typename std::enable_if<(axisCount > 0), vectn<t, math::maximum(axisCount - 1, (fsize_t)1)>>
		::type& in, const t& valueToAdd) : vectn(in)
	{
		axis[axisCount - 1] = valueToAdd;
	}

	template<typename indexType, typename = std::enable_if<std::is_integral<t>::value>>
	constexpr t& operator[](const indexType& axisIndex)
	{
		assumeInRelease((axisIndex < (indexType)axisCount) && (axisIndex >= 0));
		return this->axis[axisIndex];
	}

	template<typename indexType, typename = std::enable_if<std::is_integral<t>::value>>
	constexpr const t& operator[](const indexType& axisIndex) const
	{
		assumeInRelease((axisIndex < (indexType)axisCount) && (axisIndex >= 0));
		return this->axis[axisIndex];
	}

	constexpr t& operator[](caxisID& axisIndex)
	{
		return operator[]((fsize_t)axisIndex);
	}

	constexpr const t& operator[](caxisID& axisIndex) const
	{
		return operator[]((fsize_t)axisIndex);
	}

	addMemberName(x, (*this)[0])
		addMemberName(y, (*this)[1])
		addMemberName(z, (*this)[2])
		addMemberName(w, (*this)[3])

		constexpr t sum() const
	{
		t result = axis[0];
		for (fsize_t i = 1; i < axisCount; i++)
		{
			result += axis[i];
		}
		return result;
	}

	constexpr t volume() const
	{
		t result = axis[0];
		for (fsize_t i = 1; i < axisCount; i++)
		{
			result *= axis[i];
		}
		return result;
	}

	constexpr t lengthSquared() const
	{
		t result = math::squared(axis[0]);
		for (fsize_t i = 1; i < axisCount; i++)
		{
			result += math::squared(axis[i]);
		}
		return result;
	}

	inline t length() const
	{
		return (t)math::fastSqrt((float)lengthSquared());
	}

	inline vectn normalized() const
	{
		vectn result = *this;
		result.normalize();
		return result;
	}

	inline void normalize()
	{
		*this *= (t)math::fastInverseSqrt((float)lengthSquared());

		//to stop getrotation() from returning nan
		for (auto& axisIt : *this)
		{
			axisIt = math::clamp(axisIt, (t)-1, (t)1);
		}
	}

	//can be 0, 90,180, 270
	constexpr vectn rotateXY(cfsize_t& angle) const
	{

		cint sina = math::sinDegrees(angle);
		cint cosa = math::cosDegrees(angle);

		return vectn(
			x() * cosa + y() * sina, 
			x() * -sina + y() * cosa
		);
	}

	//0 -> 0, 1
	//pi / 2 -> 1,0
	//pi -> 0, -1
	//pi * 1.5 -> -1,0
	inline static vectn getrotatedvector(const fp& yaw)
	{
		//rotate vector over y axis(pitch)
		return vectn(sin(yaw), cos(yaw));
	}

	inline static vectn getrotatedvector(const t& yaw, const t& pitch)
	{
		//rotate vector over y axis(pitch)
		t cosp = cos(pitch), sinp = sin(pitch), siny = sin(yaw), cosy = cos(yaw);
		return vectn(siny * cosp, cosy * cosp, sinp);
	}

	//this vector has to be normalized!
	//clockwise
	//+y = up
	//x 0, y 1 : 0
	inline fp getRotation() const
	{
		fp angle = asin(x());
		if (y() < 0)
		{
			angle = math::PI - angle;
		}
		if (angle < 0) {
			angle += math::PI2;
		}
		//x 0, y 1 : 0
		return angle;
	}

	constexpr vectn absolute() const
	{
		vectn result = vectn();
		for (fsize_t i = 0; i < axisCount; i++)
		{
			result[i] = abs(axis[i]);
		}
		return result;
	}

	//axis has to be normalized!
	inline static vectn rotate(const vectn& vec, const vectn& axis, const t& angle)
	{
		return (cos(angle) * vec) + (sin(angle) * cross(axis, vec));
	}

	inline static constexpr t dot(const vectn& lhs, const vectn& rhs)
	{
		t result = t();
		for (fsize_t i = 0; i < axisCount; i++)
		{
			result += lhs[i] * rhs[i];
		}
		return result;
	}

	inline static vectn cross(const vectn& lhs, const vectn& rhs)
	{
		return vectn(
			lhs.y() * rhs.z() - lhs.z() * rhs.y(),
			lhs.z() * rhs.x() - lhs.x() * rhs.z(),
			lhs.x() * rhs.y() - lhs.y() * rhs.x());
	}

	constexpr t maximum() const
	{
		return axis[minimumIndex([](auto&& lhs, auto&& rhs) {
			return lhs > rhs;
			})];
	}

	constexpr t minimum() const
	{
		return axis[minimumIndex([](auto&& lhs, auto&& rhs) {
			return lhs < rhs;
			})];
	}

	//comparefunction should return true if left < right
	template<typename compareFunction>
	constexpr fsize_t minimumIndex(compareFunction&& function) const
	{
		fsize_t result = 0;
		for (fsize_t i = 1; i < axisCount; i++)
		{
			if (function(axis[i], axis[result]))
			{
				result = i;
			}
		}
		return result;
	}


	//operators

	constexpr bool operator ==(const vectn& other) const
	{
		for (fsize_t i = 0; i < axisCount; i++)
		{
			if (axis[i] != other[i])
			{
				return false;
			}
		}
		return true;
	}
	constexpr bool operator !=(const vectn& other) const
	{
		return !operator==(other);
	}

	constexpr vectn operator -() const
	{
		vectn result = vectn();
		for (fsize_t i = 0; i < axisCount; i++)
		{
			result[i] = -axis[i];
		}
		return result;
	}

	//used for sorting
	//returns true if pos0 is 'lower' than pos1
	//orders: 0,0 1,0 0,1 1,1
	constexpr bool operator < (const vectn& other) const noexcept
	{
		for (fsize_t i = axisCount - 1; ; i--)
		{
			if (axis[i] < other[i])
			{
				return true;
			}
			else if (axis[i] > other[i])
			{
				return false;
			}
			else if (i == 0)
			{
				break;
			}
		}
		return false;
	}

	constexpr t getDimensionalArrayIndex(const vectn& size) const
	{
		t index = (*this)[axisCount - 1];
		if constexpr (axisCount == 1)
		{
			return index;
		}
		else
		{
			for (fsize_t i = axisCount - 2; ; i--)
			{
				index *= size[i];
				index += (*this)[i];
				if (i == 0)
				{
					return index;
				}
			}
			//can't come here
			assumeInRelease(false);
		}
	}

	constexpr bool indexInBounds(const vectn& size) const
	{
		for (auto it : std::views::zip((*this), size))
		{
			if (it.template val<0>() < 0 || it.template val<0>() >= it.template val<1>())
			{
				return false;
			}
		}
		return true;
	}

#define newMacro(copySize) vectn result = vectn();
	addOperators(newMacro, vectn, constexpr)
#undef newMacro
};

addTemplateTypes(vec)

template<typename outputType = int, typename inputType, fsize_t axisCount>
constexpr vectn<outputType, axisCount> floorVector(const vectn<inputType, axisCount>& vec)
{
	vectn<outputType, axisCount> result = vectn<outputType, axisCount>();
	for (fsize_t i = 0; i < axisCount; i++)
	{
		result[i] = (outputType)math::floor(vec[i]);
	}
	return result;
}

template<typename outputType = int, typename inputType, fsize_t axisCount>
constexpr vectn<outputType, axisCount> ceilVector(const vectn<inputType, axisCount>& vec)
{
	vectn<outputType, axisCount> result = vectn<outputType, axisCount>();
	for (fsize_t i = 0; i < axisCount; i++)
	{
		result[i] = (outputType)ceil(vec[i]);
	}
	return result;
}

//https://www.omnicalculator.com/math/angle-between-two-vectors
//a and b have to be NORMALIZED
template<typename t>
inline fp angleBetween(const vect2<t>& a, const vect2<t>& b)
{
	return acos(vec2::dot(a, b));
}

//y is greater at the top
template<typename t>
constexpr bool woundClockwise(const vect2<t>& a, const vect2<t>& b, const vect2<t>& c)
{
	//counter-clockwise
	const vect2<t>& dab = b - a;
	const vect2<t>& dac = c - a;
	const t& winding = dab.x() * dac.y() - dab.y() * dac.x();
	return winding < 0;
}

//inline constexpr static std::array<int, 2> arr = std::array<int, 2>({ 2 });
//inline constexpr static vec2 vec2Test = vec2(1);
//inline constexpr static fp valueTest = vec2Test.sum();
//inline constexpr static vectn<fp, 2> lessThan1 = vec2Test - vec2(0, 4);
//inline constexpr static cvect3<fp> vectest = { 0, 1 };


constexpr veci2 directionVectors2D[directionCount<2>()]
{
	cveci2(-1,0),
	cveci2(1,0),
	cveci2(0,-1),
	cveci2(0,1),
};