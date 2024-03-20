#include "GlobalFunctions.h"
#pragma once
#include "file/endian.h"
#include "array/wstring.h"
#include "serialization.h"
#include "iserializer.h"
#include "math/vectn.h"
#include "math/uuid.h"

typedef std::basic_iostream<char> defaultStreamType;

template <typename streamType = defaultStreamType>
struct serializer :iSerializer
{
	//the endianness of the file the serializer is interacting with
	const endianness fileEndianness;

	streamType& stream;
	serializer(streamType& stream, cbool& write, const endianness& fileEndianness = currentEndianness) :
		iSerializer(write), stream(stream), fileEndianness(fileEndianness) {}

	template<typename valueType, typename = std::enable_if_t<is_endian_convertable_v<valueType>>>
	inline bool serialize(valueType& value) const
	{
		if (write)
		{
			if (fileEndianness != currentEndianness)
			{
				valueType correctEndian = value;//do not modify the value which is passed by reference

				convertEndian(correctEndian);

				return (bool)stream.write(castout(&correctEndian), sizeof(valueType));
			}
			else
			{
				return (bool)stream.write(castout(&value), sizeof(valueType));
			}
		}
		else
		{
			if (stream.read(castin(&value), sizeof(valueType)))
			{
				if (fileEndianness != currentEndianness)
				{
					convertEndian(value);
				}
				return true;
			}
		}
		return false;
	}

	template<typename valueType>
	inline bool serialize(valueType* valuesPointer, const size_t& arraySize) const
	{
		if constexpr (is_endian_convertable_v<valueType>) {

			if (write)
			{
				if (fileEndianness != currentEndianness && sizeof(valueType) > 1)
				{
					const valueType* const endPtr = valuesPointer + arraySize;
					for (; valuesPointer < endPtr; )
					{
						valueType correctEndian = *valuesPointer++;
						convertEndian(correctEndian);
						if (!stream.write(castout(&correctEndian), sizeof(valueType))) {
							return false;
						}
					}
					return true;
				}
				else
				{
					return (bool)stream.write(castout(valuesPointer), sizeof(valueType) * arraySize);
				}
			}
			else
			{
				if (fileEndianness != currentEndianness && sizeof(valueType) > 1)
				{
					const valueType* const endPtr = valuesPointer + arraySize;
					for (; valuesPointer < endPtr; )
					{
						if (!stream.read(castin(valuesPointer), sizeof(valueType))) {
							return false;
						}
						convertEndian(*valuesPointer++);
					}
					return true;
				}
				else
				{
					return (bool)stream.read(castin(valuesPointer), sizeof(valueType) * arraySize);
				}
			}
		}
		else {
			const valueType* const endPtr = valuesPointer + arraySize;

			for (; valuesPointer < endPtr; )
			{
				serialize(*valuesPointer++);
			}
			return true;
		}
	}

	template<typename valueType, size_t arraySize, typename = std::enable_if_t<is_endian_convertable_v<valueType>>>
	inline bool serialize(valueType(&values)[arraySize]) const
	{
		return serialize(&values[0], arraySize);
	}
	inline bool serialize(uuid& id) const
	{
		return serialize(id.idInts);
	}
	template<typename valueType>
	inline bool serialize(std::vector<valueType>& values) const
	{
		size_t size = write ? values.size() : 0;
		if (!serialize(size)) {
			return false;
		}
		if (!write) {
			values.resize(size);
		}
		return size ? serialize(&values[0], values.size()) : false;
		//return true;
	}

	template<typename valueType, fsize_t axisCount, typename = std::enable_if_t<is_endian_convertable_v<valueType>>>
	inline bool serialize(vectn<valueType, axisCount>& value) const
	{
		return serialize(value.axis, axisCount);
		//for (fsize_t i = 0; i < axisCount; i++)
		//{
		//	if (!serialize(value.axis[i]))
		//	{
		//		return false;
		//	}
		//}
		//return true;
	}

	template<typename sizeType = int>
	inline void serializeString(std::string& str) const
	{
		sizeType size;
		if (write)
		{
			size = (sizeType)str.size();
			serialize(size);
		}
		else
		{
			serialize(size);
			str = std::string((size_t)size, L'#');
		}
		serialize(&str[0], str.length());
	}

	inline void serializeStringUntilZero(std::string& str) const
	{
		int8_t stringEnd = 0;
		if (write)
		{
			serialize(&str[0], str.length());
			serialize(stringEnd);
		}
		else
		{
			str = std::string();

			int8_t nextByte;
			while (true)
			{
				serialize(nextByte);
				if (nextByte == stringEnd)
				{
					return;
				}
				else
				{
					str += nextByte;
				}
			}
		}
	}

	template<typename sizeType = int>
	inline void serializeWStringAsString(std::wstring& str) const
	{
		std::string utf8String;
		if (write)
		{
			utf8String = WStringToString(str);
			serializeString<sizeType>(utf8String);
		}
		else
		{
			serializeString<sizeType>(utf8String);
			str = stringToWString(utf8String);
		}
	}
};
typedef serializer<defaultStreamType> streamSerializer;