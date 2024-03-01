#pragma once
#include "array/fastarray.h"
#include "math/vectn.h"

constexpr fp sigmoidMin = 0;
constexpr fp sigmoidMax = 1;
constexpr fp sigmoidMid = (sigmoidMax + sigmoidMin) * 0.5;

struct aiRawInputData
{
	fp* values = nullptr;
	size_t size = 0;

	aiRawInputData(cfsize_t& size = 1) : size(size) {}

};


struct aiInputData : aiRawInputData
{
	aiInputData(cfsize_t& size = 1) : aiRawInputData(size) {}

	template<typename t>
	constexpr void setValue(const t& value, cfsize_t& index = 0) const
	{
		if constexpr (std::is_same<t, fp>::value)
		{
			if constexpr (isDebugging)
			{
				if ((value < sigmoidMin) || (value > sigmoidMax))
				{
					throw "value not in range";
				}
			}

			values[index] = value;
		}
		else if constexpr (std::is_same<t, bool>::value)
		{
			values[index] = value ? sigmoidMax : sigmoidMin;
		}
	}

	template<typename t>
	constexpr t getValue(cfsize_t& index = 0) const
	{
		if constexpr (std::is_same<t, fp>::value)
		{
			return values[index];
		}
		else if constexpr (std::is_same<t, bool>::value)
		{
			return values[index] >= sigmoidMid ? true : false;
		}
	}

	template<typename t>
	constexpr fastArray<t> getValues() const
	{
		fastArray<t> output = fastArray<t>(values.size);

		for (size_t i = 0; i < values.size; i++)
		{
			output[i] = getValue(i);
		}
	}

	template<typename t>
	constexpr void setValues(const fastArray<t>& values) const
	{
		for (size_t i = 0; i < values.size; i++)
		{
			setValue(values[i], i);
		}
	}

	template<typename t, size_t axisCount>
	constexpr void setValue(cvectn<t, axisCount>& value) const
	{
		for (size_t i = 0; i < axisCount; i++)
		{
			setValue(value[i], i);
		}
	}

	//set one of the values to 1 and the rest to 0
	template<typename valueType, typename containerType>
	constexpr void setValueOption(const valueType& currentOption, const containerType& possibleOptions, cfsize_t& index = 0)
	{
		cfsize_t& optionCount = possibleOptions.size();

		for (fsize_t i = 0; i < optionCount; ++i)
		{
			setValue(currentOption == possibleOptions[i], i + index);
		}
	}

	//get the value with the highest possibility
	template<typename valueType, typename containerType>
	constexpr valueType getValueOption(const containerType& possibleOptions, cfsize_t& index = 0)
	{
		cfsize_t& optionCount = possibleOptions.size();

		fsize_t highestIndex = 0;
		fp highestValue = values[index];

		for (fsize_t i = 1; i < optionCount; ++i)
		{
			cfp& currentValue = values[i + index];
			if (currentValue > highestValue)
			{
				highestIndex = i;
				highestValue = currentValue;
			}
		}
		return possibleOptions[highestIndex];
	}
};

struct aiOutputData
{
	aiInputData aiGuess = aiInputData();
	aiInputData correctAnswer = aiInputData();

	aiOutputData(cfsize_t& size = 1) : aiGuess(size), correctAnswer(size) {}

	template<typename t>
	inline void serializeWithOutput(cbool& isCorrectAnswer, t& output, cfsize_t& index = 0) const
	{
		if (isCorrectAnswer)
		{
			correctAnswer.setValue(output, index);
		}
		else 
		{
			output = aiGuess.getValue<t>(index);
		}
	}

};