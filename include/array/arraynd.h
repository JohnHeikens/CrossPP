#pragma once
#include "math/mathfunctions.h"
#include "interface/idestructable.h"
#include "filesystem/iserializable.h"
#include "fastarray.h"
#include "math/cropline.h"
#include "math/mattnxn.h"
#include <list>
#include "math/graphics/brush/brush.h"
#include "math/graphics/brush/transformbrush.h"
#include "math/axis.h"

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
			ptr += size.x();
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
			if (index > size.y())
			{
				throw std::exception("index out of range");
			}
		}
		return baseArray + index * size.x();
	}

#define newMacro(copySize) arraynd result = arraynd((copySize).size);
	addOperators(newMacro, arraynd, inline)
#undef newMacro

		//in = y
		//adds the sum of each x row to one element of the output
		inline fastArray<t> multPointMatrix(const fastArray<t>& in) const
	{
		fastArray<t> result = fastArray<t>(size.x());
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

	//fill a row of pixels with a brush
	template<typename brush0Type>
	inline void fillRowUnsafe(cfsize_t& rowY, cfsize_t& minX, cfsize_t& maxX, const brush0Type& b) const
	{
		t* const rowPtr = baseArray + rowY * size.x();
		t* const endPtr = rowPtr + maxX;

		typedef brush0Type::inputType vectorType;
		typedef vectorType::axisType axisType;

		vectorType pos = vectorType((axisType)minX, (axisType)rowY);

		for (t* ptr = rowPtr + minX; ptr < endPtr; ptr++, pos.x()++)
		{
			*ptr = b.getValue(pos);
		}
	}
	template<typename brush0Type>
	inline void fillRowUnsafe(cfsize_t& rowY, cfsize_t& minX, cfsize_t& maxX, const transformBrush<brush0Type>& b) const
	{
		t* const rowPtr = baseArray + rowY * size.x();
		t* const endPtr = rowPtr + maxX;


		vec2 pos = b.modifiedTransform.multPointMatrix(vec2((fp)minX, (fp)rowY));
		vec2 step = b.modifiedTransform.getStep<2>(axisID::x);

		for (t* ptr = rowPtr + minX; ptr < endPtr; ptr++, pos += step)
		{
			*ptr = b.baseBrush.getValue(pos);
		}
	}

	template<typename brush0Type>
	inline void fillRow(cint& rowY, cint& minX, cint& maxX, const brush0Type& b) const
	{
		if ((rowY >= 0) && (maxX > 0))
		{
			fillRowUnsafe(rowY, (fsize_t)math::maximum(minX, 0), math::minimum((fsize_t)maxX, size.x()), b);
		}
	}

	template<typename brush0Type>
	inline void fillRow(cint& rowY, cfp& minX, cfp& maxX, const brush0Type& b) const
	{
		//ceil rule
		fillRow(rowY, math::maximum((int)ceil(minX), 0), math::minimum((int)ceil(maxX), size.x()), b);
	}
	template<typename brush0Type>
	inline void fillRectangleUnsafe(crectanglet2<fsize_t>& rect, const brush0Type& b) const
	{
		cvect2<fsize_t>& pos11 = rect.pos1();
		for (fsize_t currentY = rect.y(); currentY < pos11.y(); currentY++)
		{
			fillRowUnsafe<brush0Type>(currentY, rect.pos0.x(), pos11.x(), b);
		}
	}

	template<typename brush0Type>
	inline void fillRectangle(rectanglei2 rect, const brush0Type& b) const
	{
		if (getClientRect().cropClientRect(rect))
		{
			fillRectangleUnsafe(crectanglet2<fsize_t>(rect), b);
		}
	}

	template<typename brush0Type>
	inline void fillRectangle(crectangle2& rect, const brush0Type& b) const
	{
		fillRectangle(ceilRectangle(rect), b);
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
	//smaller file size, faster to read and write
	//use this only with lots of repeating values!
	inline void serializeEfficient(const streamSerializer& s) const
	{
		cint arraySize = size.volume();
		if (arraySize)
		{
			int index = 0;
			int currentLength = 0;
			t currentValue = baseArray[0];
			while (index < arraySize)
			{
				if (s.write)
				{
					const t newValue = baseArray[index];
					if (currentValue == newValue)
					{
						currentLength++;
					}
					else
					{
						s.serialize(currentValue);
						s.serialize(currentLength);
						currentValue = newValue;
						currentLength = 1;
					}
					index++;
				}
				else
				{
					s.serialize(currentValue);
					s.serialize(currentLength);
					std::fill(&baseArray[index], &baseArray[index + currentLength], currentValue);
					index += currentLength;
				}
			}
			if (s.write)
			{
				s.serialize(currentValue);
				s.serialize(currentLength);
			}
		}
	}
	constexpr bool inBounds(cvecin<axisCount>& pos) const
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
	inline bool inBounds(crectanglei2& rect) const
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
			assumeInRelease(inBounds(pos));
		}
		return baseArray[pos.getDimensionalArrayIndex(size)];
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

		for (auto axisIt : zip(clamped, pos, size))
		{
			axisIt.val<0>() = (axisIt.val<1>() < axisIt.val<2>()) ? axisIt.val<1>() : (axisIt.val<2>() - 1);
		}

		return getValueUnsafe(clamped);
	}

	inline t getValueClampedToEdge(cvecin<axisCount>& pos) const
	{
		cvecin<axisCount> clamped = cvecin<axisCount>();
		for (auto axisIt : zip(clamped, pos))
		{
			axisIt.val<0>() = (axisIt.val<1>() < 0) ? 0 : axisIt.val<1>();
		}

		return getValueClampedToEdgePositive(cvectn<fsize_t, axisCount>(clamped));
	}

	inline void floodFill(cveci2& pos, const t& value)
	{
		//	1. If target is equal to replacement, return.
		t target = getValue(pos);
		if (target == value)return;
		//3. Set the value of node to replacement.
		this->setValue(pos, value);
		//4. Set Q to the empty queue.
		std::list<veci2> Q = std::list<veci2>();
		//5. Add node to the end of Q.
		Q.push_back(pos);
		//6. While Q is not empty:
		while (Q.size() > 0)
		{
			//7. Set n equal to the first element of Q.
			veci2 n = *Q.begin();
			//8.     Remove first element from Q.
			Q.pop_front();
			//9.     If the value of the node to the west of n is target,
			if (n.x() > 0 && getValue(n.x() - 1, n.y()) == target) {
				//set the value of that node to replacement and add that node to the end of Q.
				setValue(n.x() - 1, n.y(), value);
				Q.push_back(veci2(n.x() - 1, n.y()));
			}
			//10.     If the value of the node to the east of n is target,
			if (n.x() < size.x() - 1 && getValue(n.x() + 1, n.y()) == target) {
				//set the value of that node to replacement and add that node to the end of Q.
				setValue(n.x() + 1, n.y(), value);
				Q.push_back(veci2(n.x() + 1, n.y()));
			}
			//11.     If the value of the node to the north of n is target,
			if (n.y() > 0 && getValue(n.x(), n.y() - 1) == target) {
				//set the value of that node to replacement and add that node to the end of Q.
				setValue(n.x(), n.y() - 1, value);
				Q.push_back(veci2(n.x(), n.y() - 1));
			}
			//12.     If the value of the node to the south of n is target,
			if (n.y() < size.y() - 1 && getValue(n.x(), n.y() + 1) == target) {
				//set the value of that node to replacement and add that node to the end of Q.
				setValue(n.x(), n.y() + 1, value);
				Q.push_back(veci2(n.x(), n.y() + 1));
			}
			//13. Continue looping until Q is exhausted.
		}
		//14. Return.
	}

	inline void copyArrayUnsafe(crectanglet2<fsize_t>& destinationRect, cfsize_t& otherArrayWidth, const t* otherArrayPointer) const
	{
		cvect2<fsize_t>& pos11 = destinationRect.pos1();

		t* const& xPtr = baseArray + destinationRect.x();
		const t* const& endPtr = xPtr + pos11.y() * size.x();

		for (
			t* ptr = xPtr + (destinationRect.pos0.y() * size.x());
			ptr < endPtr;
			ptr += size.x(), otherArrayPointer += otherArrayWidth)
		{
			std::copy(otherArrayPointer, otherArrayPointer + destinationRect.size.x(), ptr);
		}
	}

	inline void copyArrayUnsafe(crectanglet2<fsize_t>& sourceRect, cvect2<fsize_t>& destinationPosition, const arraynd& source) const
	{
		copyArrayUnsafe(crectanglei2(destinationPosition, sourceRect.size), source.size.x(), source.baseArray + sourceRect.pos0.x() + sourceRect.pos0.y() * source.size.x());
	}

	inline void copyArray(cveci2& destinationPosition, const arraynd& source) const
	{
		//crop rectangle
		rectanglei2 rect = rectanglei2(destinationPosition.x(), destinationPosition.y(), source.size.x(), source.size.y());
		rect.crop(crectangletn<int, 2>(getClientRect()));
		copyArrayUnsafe(crectanglet2<fsize_t>(rect), source.size.x(), source.baseArray);
	}

	inline void copyArray(crectanglei2& sourceRect, cveci2& destinationPosition, const arraynd& source) const
	{
		rectanglei2 croppedSourceRect = sourceRect;
		if (!crectanglei2(source.getClientRect()).cropClientRect(croppedSourceRect))
		{
			return;
		}

		cveci2 croppedDestinationPosition = destinationPosition + (croppedSourceRect.pos0 - sourceRect.pos0);

		rectanglei2 croppedDestinationRect = crectanglei2(croppedDestinationPosition, croppedSourceRect.size);
		crectanglei2(getClientRect()).cropClientRect(croppedDestinationRect);

		cveci2 croppedSourcePosition = croppedSourceRect.pos0 + (croppedDestinationRect.pos0 - croppedDestinationPosition);
		copyArrayUnsafe(crectanglet2<fsize_t>(croppedDestinationRect), source.size.x(), source.baseArray + croppedSourcePosition.x() + croppedSourcePosition.y() * source.size.x());
	}

	inline arraynd expanded(crectanglei2& newRect) const
	{
		arraynd resizedArray = arraynd(newRect.size, true);
		//copy all that can be copied
		resizedArray.copyArray(newRect, cveci2(), *this);
		return resizedArray;
	}

	inline void expandToContain(cveci2& relativePosition)
	{
		if (!inBounds(relativePosition))
		{
			rectanglei2 newRelativeRect = rectanglei2(getClientRect());
			newRelativeRect.expandToContain(crectanglei2(relativePosition, cveci2(1)));
			const arraynd& expandedArray = expanded(newRelativeRect);
			*this = expandedArray;
		}
	}

	template<typename brush0Type>
	inline void fillRectangleBorders(crectanglei2& rect, cint& borderThickness, const brush0Type& b) const
	{
		fillRectangle(crectanglei2(rect.x(), rect.y(), rect.size.x(), borderThickness), b);
		fillRectangle(crectanglei2(rect.x(), rect.y() + rect.size.y() - borderThickness, rect.size.x(), borderThickness), b);
		fillRectangle(crectanglei2(rect.x(), rect.y(), borderThickness, rect.size.y()), b);
		fillRectangle(crectanglei2(rect.x() + rect.size.x() - borderThickness, rect.y(), borderThickness, rect.size.y()), b);
	}
	//expands inward
	template<typename brush0Type>
	inline void fillRectangleBorders(cveci2& pos00, cveci2& pos11, cint& borderThickness, const brush0Type& b) const
	{
		fillRectangleBorders(crectanglei2(pos00, pos11 - pos00 + 1), borderThickness, b);
	}
	template<typename compareFunction>
	inline bool findRayCast(cvec2& p0, cvec2& p1, veci2& resultingBlockPos, veci2& adjacentBlockPosition, vec2& exactIntersection, const compareFunction&& function)
	{
		//https://www.redblobgames.com/grids/line-drawing.html
			//http://www.cse.yorku.ca/~amana/research/grid.pdf
		cvec2 d0to1 = p1 - p0;//dx
		cveci2 sign = veci2(d0to1.x() > 0 ? 1 : -1, d0to1.y() > 0 ? 1 : -1);

		//not dx * sign_x, because you can get -0 and -inf
		cvec2 absd0to1 = d0to1.absolute();//nx
		veci2 currentPosition = veci2((int)floor(p0.x()), (int)floor(p0.y()));
		int dimension = -1;
		cvec2 delta = { 1 / absd0to1.x(),1 / absd0to1.y() };//deltax
		//vx
		vec2 progress = {
		d0to1.x() > 0 ? currentPosition.axis[0] + 1 - p0.x() : p0.x() - currentPosition.axis[0],
		d0to1.y() > 0 ? currentPosition.axis[1] + 1 - p0.y() : p0.y() - currentPosition.axis[1]
		};
		progress *= delta;//'divide' by total length
		int count = 0;
		bool looping = false;//initialize to get the warning to shut up
		t hit = t();
		do {
			//check point
			hit = getValue(currentPosition);
			if (function(hit))
			{
				break;
			}
			dimension = progress.x() < progress.y() ? 0 : 1;
			currentPosition.axis[dimension] += sign.axis[dimension];//step in the active dimenstion
			looping = progress.axis[dimension] <= 1;
			progress.axis[dimension] += delta.axis[dimension];//update progress
		} while (looping);
		//hit = getValue(currentPosition);
		bool visible = function(hit);
		resultingBlockPos = currentPosition;
		adjacentBlockPosition = currentPosition;
		if (dimension != -1)
		{
			adjacentBlockPosition.axis[dimension] -= sign.axis[dimension];
			if (!visible)
			{
				resultingBlockPos = adjacentBlockPosition;
			}
		}
		if (visible)
		{
			if (dimension == -1)
			{
				exactIntersection = p0;
			}
			else
			{
				//calculate the exact intersection point.
				exactIntersection.axis[dimension] = currentPosition.axis[dimension];
				if (sign.axis[dimension] == -1)
				{
					exactIntersection.axis[dimension]++;//this axis has to be increased because the ray hit on the plus side of the block.
				}
				cfp distance = abs(exactIntersection.axis[dimension] - p0.axis[dimension]);
				int otherAxis = dimension == 0 ? 1 : 0;
				fp part = distance / absd0to1.axis[dimension];//the part of the ray that has been marched
				fp exactCoordinate = p0.axis[otherAxis] + part * d0to1.axis[otherAxis];
				exactIntersection.axis[otherAxis] = exactCoordinate;
			}
		}
		else
		{
			exactIntersection = p1;
		}
		return visible;
	}
	//to is NOT compared compared!
	//the 'elements' outside the border are not compared
	template<typename compareFunction>
	inline bool find2dBox(cveci2& pos00, cveci2& pos11, const compareFunction&& function)
	{
		for (veci2 position = pos00; position.y() < pos11.y(); position.y()++)
		{
			for (position.x() = pos00.x(); position.x() < pos11.x(); position.x()++)
			{
				if (function(getValue(position.x(), position.y())))
				{
					return true;
				}
			}
		}
		return false;
	}

	//x, y: pos00 position
	//w, h: width, height
	template<typename brush0Type>
	inline void fillEllipse(crectangle2& rect, const brush0Type& b) const
	{
		rectanglei2 croppedRect = ceilRectangle(rect);

		//+1 for also filling the last pixel
		croppedRect.h()++;

		//crectanglei2& croppedRect = ceiledRect.cropped(crectanglei2(cveci2(), size));
		if (crectanglei2(cveci2(), size).cropClientRect(croppedRect))
		{
			cveci2& croppedPos1 = croppedRect.pos1();
			//crop

			cfp& midx = rect.x() + rect.w() * .5;
			cfp& midy = rect.y() + rect.h() * .5;
			cfp& multx = 2.0 / rect.w();
			cfp& multy = 2.0 / rect.h();

			for (int j = croppedRect.y(); j < croppedPos1.y(); j++)
			{
				//circle equation:
				//x * x + y * y = r * r
				//ellipse equation:
				//(dx * multx)^2 + (dy*multy)^2 = 1
				//(dx * multx)^2 = 1 - (dy * multy) ^2
				//dx * multx = sqrt(1 - (dy * multy) ^ 2)
				//dx = sqrt(1 - (dy * multy) ^2) / multx
				cfp val = 1 - math::squared((midy - j) * multy);
				if (val > 0) {
					cfp dx = sqrt(val) / multx;

					cfp& fromX = midx - dx;
					cfp& toX = midx + dx;

					//0.5 to 1.5:
					//only fill two pixels
					cint& minX = math::maximum((int)ceil(midx - dx), croppedRect.x());
					cint& maxX = math::minimum((int)floor(midx + dx) + 1, croppedPos1.x());//+1 for also filling the last pixel

					t* ptr = baseArray + j * size.x() + minX;

					fillRow(j, minX, maxX, b);
				}
			}
		}
	}

	template<typename brush0Type>
	inline void fillEllipseCentered(cvec2& pos, cvec2& size, const brush0Type& b) const
	{
		fillEllipse(crectangle2(pos - size * 0.5, size), b);
	}



	//draw a line from p0 to p1
	//source:
	//http://blog.ruofeidu.com/bresenhams-line-algorithm-in-c/
	//also worth checking out:
	//https://www.redblobgames.com/grids/line-drawing.html
	//https://www.geeksforgeeks.org/bresenhams-line-generation-algorithm/
	template<typename brush0Type>
	inline void fillLine(vec2 p0, vec2 p1, const brush0Type& b) const
	{
		if (cropLine(p0, p1, crectangle2(vec2(), vec2(size - 1))))
		{
			fillLineUnsafe(p0, p1, b);
		}
	}

	template<typename brush0Type>
	inline void fillLineUnsafe(cvec2& p0, cvec2& p1, const brush0Type& b) const
	{
		veci2 currentPosition = veci2((int)p0.x(), (int)p0.y());
		veci2 endPosition = veci2((int)p1.x(), (int)p1.y());
		veci2 difference = veci2(abs(endPosition.x() - currentPosition.x()), abs(endPosition.y() - currentPosition.y()));
		veci2 sign = veci2(currentPosition.x() < endPosition.x() ? 1 : -1, currentPosition.y() < endPosition.y() ? 1 : -1);
		int err = difference.x() - difference.y();

		while (true)
		{
			setValue(currentPosition, b.getValue(currentPosition));
			if (currentPosition == endPosition) return;
			cint& e2 = err * 2;//will result in a bit shift operation because of the optimizer.
			if (e2 > -difference.y())
			{
				err -= difference.y();
				currentPosition.x() += sign.x();
			}
			if (e2 < difference.x())
			{
				err += difference.x();
				currentPosition.y() += sign.y();
			}
		}
	}

	//advanced images
	//ONLY TRANSFORMS THE RECTANGLE, NOT THE COLORS

	//the transform should be from the rectangle to the screen

	//shifted so the first point is the lowest point
	//clockwise!
	template<typename brush0Type>
	inline void fillPolygonShiftedUnsafe(const fastArray<vec2>& shiftedPositions, cfp& minY, cfp& maxY, const brush0Type& b) const
	{
		//0 = left, 1 = right
		fsize_t nextPointIndex[2]{ 0,0 };
		cint windDirection[2]{ 1,-1 };//left: 1 (clockwise), right: -1 (counter clockwise)
		fp slope[2]{ 0,0 };
		cfsize_t sideCount = 0x2;
		fp currentX[2]{ 0,0 };

		cfsize_t maxYCropped = (fsize_t)math::minimum(math::ceil<fp, fsize_t>(maxY), size.y());
		//initialize the first two lines by default
		for (fsize_t currentY = (fsize_t)math::maximum((int)ceil(minY), 0); currentY < maxYCropped; currentY++)
		{
			for (fsize_t side = 0; side < sideCount; side++)
			{
				bool changed = false;
				while (currentY >= shiftedPositions[nextPointIndex[side]].y())
				{
					//maybe there is a straight line or something
					if ((nextPointIndex[side] == 0) && (windDirection[side] == -1))
					{
						nextPointIndex[side] = (fsize_t)shiftedPositions.size - 1;
					}
					else
					{
						nextPointIndex[side] += windDirection[side];
					}
					changed = true;
				}
				//passed point, get next point
				if (changed)
				{
					cvec2& selectedPos0 = shiftedPositions[(nextPointIndex[side] - windDirection[side]) % shiftedPositions.size];
					cvec2& selectedPos1 = shiftedPositions[nextPointIndex[side]];
					//update
					slope[side] = (selectedPos1.x() - selectedPos0.x()) / (selectedPos1.y() - selectedPos0.y());
					currentX[side] = selectedPos0.x() + slope[side] * (currentY - selectedPos0.y());
				}
				else
				{
					currentX[side] += slope[side];
				}
			}
			//CAUTION WITH SLOPE
			cint minX = math::maximum((int)ceil(currentX[0]), 0);
			cint maxX = math::maximum((int)ceil(currentX[1]), 0);
			//draw line
			fillRow(currentY, minX, maxX, b);
		}
	}

	//positions can't overlap each other in the x axis
	template<typename brush0Type>
	inline void fillPolygon(const fastArray<vec2>& positions, const brush0Type& b) const
	{
		//find highest y pos
		fp highestY = 0;
		fp lowestY = (fp)size.y();
		fsize_t lowestIndex = 0;
		//get boundaries and highest y index
		for (fsize_t index = 0; index < positions.size; index++)
		{
			if (positions[index].y() > highestY)
			{
				highestY = positions[index].y();
			}
			if (positions[index].y() < lowestY)
			{
				lowestY = positions[index].y();
				lowestIndex = index;
			}
		}

		if (highestY > 0 && lowestY < size.y())
		{
			cbool& clockWise = woundClockwise(positions[0], positions[1], positions[2]);
			//shift
			fastArray<vec2> shiftedArray((fsize_t)positions.size, false);
			for (fsize_t index = 0; index < positions.size; index++)
			{
				fsize_t shiftedIndex = index;

				//do not swap these
				if (lowestIndex)
				{
					shiftedIndex = (shiftedIndex < lowestIndex ? shiftedIndex + (fsize_t)positions.size : shiftedIndex) - lowestIndex;
				}
				//0 has to stay at 0
				//flip all others
				if (shiftedIndex && !clockWise)
				{
					shiftedIndex = (fsize_t)positions.size - shiftedIndex;
				}
				shiftedArray[shiftedIndex] = positions[index];
			}
			fillPolygonShiftedUnsafe(shiftedArray, lowestY, highestY, b);
		}
	}

	template<typename brush0Type>
	inline void fillTransformedRectangle(rectangle2 brushRect, cmat3x3& transform, const brush0Type& b) const
	{
		if (transform.isStraight())//simple check which can save a lot of time
		{
			fillRectangle(transform.multRotatedRectMatrix(brushRect), b);
		}
		else {
			//expand the brushrect, so it contains the last row of pixels too
			//brushRect.size += 1;
			fastArray<vec2> positions({
				transform.multPointMatrix(brushRect.pos0),
				transform.multPointMatrix(brushRect.pos01()),
				transform.multPointMatrix(brushRect.pos1()),
				transform.multPointMatrix(brushRect.pos10()),
				});
			//check for clockwise
			fillPolygon(positions, b);
		}
	}
};



#undef addOperator

template<typename t>
using array2d = arraynd<t, 2>;

template<typename t>
using array3d = arraynd<t, 3>;