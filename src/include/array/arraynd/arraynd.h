#pragma once
#include "math/mathFunctions.h"
#include "interface/idestructable.h"
#include "filesystem/iserializable.h"
#include "array/fastarray.h"
#include "math/cropline.h"
#include "math/mattnxn.h"
#include "math/graphics/brush/brush.h"
#include "math/graphics/brush/transformbrush.h"
#include "math/axis.h"
#include "math/graphics/brush/colorMixer.h"
#include "math/vector/vectorfunctions.h"

template<typename t, fsize_t axisCount>
struct arraynd :IDestructable
{
	vectn<fsize_t, axisCount> size = vectn<fsize_t, axisCount>();
	t* baseArray = nullptr;

	constexpr arraynd(cvectn<fsize_t, axisCount>& size, t* const& baseArray) :size(size), baseArray(baseArray) {}

	constexpr arraynd(cvectn<fsize_t, axisCount>& size = cvect2<fsize_t>(), cbool& initializeToDefault = true) :
		arraynd(size, initializeToDefault ? new t[size.volume()]() : new t[size.volume()]) {}

	constexpr arraynd(const std::vector<t>& elements, cvectn<fsize_t, axisCount>& size) :
		baseArray(new t[size.volume()]), size(size)
	{
		if constexpr (isDebugging)
		{
			if (size.volume() < elements.size())
			{
				throw "too much elements for this size";
			}
		}

		std::copy(elements.begin(), elements.end(), baseArray);
	}

	//so [0][1] = [0][1]
	//[y][x]
	constexpr arraynd(const fastArray<fastArray<t>> elements) :
		arraynd(cvect2<fsize_t>(elements.size ? elements[0].size : 0, elements.size))
	{
		t* ptr = baseArray;
		for (const fastArray<t>& arr : elements)
		{
			std::copy(arr.begin(), arr.end(), ptr);
			ptr += size.x;
		}
	}

	inline virtual ~arraynd() override
	{
		if (baseArray)
		{
			delete[] baseArray;
			baseArray = nullptr;
		}
	}

	inline t* begin() const
	{
		return baseArray;
	}
	inline t* end() const
	{
		return baseArray + size.volume();
	}

	inline arraynd(const arraynd& other) :
		size(other.size),
		baseArray(new t[other.size.volume()])
	{
		std::copy(other.begin(), other.end(), begin());
	}

	void swap(arraynd& with)
	{
		std::swap(baseArray, with.baseArray);
		std::swap(size, with.size);
	}

	//this operator is necessary!!!
	inline arraynd& operator=(arraynd copy)
	{
		this->swap(copy);
		return *this;
	}

	//CAUTION!
	//[y][x]
	inline t* operator[](cfsize_t& index) const
	{
		if constexpr (isDebugging)
		{
			if (index > size.y)
			{
				throw std::out_of_range("index out of range");
			}
		}
		return baseArray + index * size.x;
	}

#define newMacro(copySize) arraynd result = arraynd((copySize).size);
	addOperators(newMacro, arraynd, inline)
#undef newMacro

		//in = y
		//adds the sum of each x row to one element of the output
		inline fastArray<t> multPointMatrix(const fastArray<t>& in) const
	{
		fastArray<t> result = fastArray<t>(size.x);
		for (fsize_t toIndex = 0; toIndex < result.size; toIndex++)
		{
			fsize_t fromIndex = 0;
			for (; fromIndex < in.size; fromIndex++)
			{
				result += in[fromIndex] * (*this)[fromIndex][toIndex];
			}
			//the extra 1's to add to the vector
			for (; fromIndex < result.size; fromIndex++)
			{
				result += (*this)[fromIndex][toIndex];
			}
		}
		return result;
	}

	inline rectanglei2 getClientRect() const
	{
		return rectanglei2(size);
	}

	//does not write size to stream!
	inline void serialize(const streamSerializer& s) const
	{
		s.serialize(baseArray, size.volume());
	}

	template <typename axisType>
	constexpr bool inBounds(cvectn<axisType, axisCount>& pos) const
	{
		for (fsize_t i = 0; i < axisCount; i++)
		{
			if ((pos[i] < 0) || ((fsize_t)pos[i] >= size[i]))
			{
				return false;
			}
		}
		return true;
	}
	constexpr bool inBounds(crectanglein<axisCount>& rect) const
	{
		cveci2 pos11 = rect.pos1();

		for (fsize_t i = 0; i < axisCount; i++)
		{
			if ((rect.pos0[i] < 0) || (pos11[i] > size[i]))
			{
				return false;
			}
		}
		return true;
	}

	inline constexpr void fill(const t& value) const
	{
		std::fill(baseArray, baseArray + size.volume(), value);
	}

	inline t& getValueReferenceUnsafe(cvectn<fsize_t, axisCount>& pos) const
	{
		if constexpr (isDebugging)
		{
			if (!inBounds(pos))
			{
				throw "position out of bounds";
			}
		}
		else
		{
#ifndef __clang__
			assumeInRelease(inBounds(pos));
#endif
		}
		return baseArray[singleDimensionalIndex(pos, size)];
	}

	inline t getValueUnsafe(cvectn<fsize_t, axisCount>& pos) const
	{
		return getValueReferenceUnsafe(pos);
	}

	inline t getValue(cvecin<axisCount>& pos) const
	{
		return inBounds(pos) ? getValueUnsafe((cvectn<fsize_t, axisCount>)pos) : t();
	}

	inline void setValueUnsafe(cvectn<fsize_t, axisCount>& pos, const t& value) const
	{
		getValueReferenceUnsafe(pos) = value;
	}
	inline void setValue(cvecin<axisCount>& pos, const t& value) const
	{
		if (inBounds(pos))
		{
			setValueUnsafe((cvectn<fsize_t, axisCount>)pos, value);
		}
	}

	inline t getValueClampedToEdgePositive(cvectn<fsize_t, axisCount>& pos) const
	{
		vectn<fsize_t, axisCount> clamped = cvectn<fsize_t, axisCount>();

		for (auto axisIt : std::views::zip(clamped, pos, size))
		{
			std::get<0>(axisIt) = (std::get<1>(axisIt) < std::get<2>(axisIt)) ? std::get<1>(axisIt) : (std::get<2>(axisIt) - 1);
		}

		return getValueUnsafe(clamped);
	}

	inline t getValueClampedToEdge(cvecin<axisCount>& pos) const
	{
		cvecin<axisCount> clamped = cvecin<axisCount>();
		for (auto axisIt : std::views::zip(clamped, pos))
		{
			std::get<0>(axisIt) = (std::get<1>(axisIt) < 0) ? 0 : std::get<1>(axisIt);
		}

		return getValueClampedToEdgePositive(cvectn<fsize_t, axisCount>(clamped));
	}
};



#undef addOperator

template<typename t>
using array2d = arraynd<t, 2>;

template<typename t>
using array3d = arraynd<t, 3>;