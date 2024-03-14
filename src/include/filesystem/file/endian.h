#pragma once
#include <cstdint>
#include <utility>

//no pointers, const values and non-fundamental types accepted
template<typename t>
inline constexpr bool is_endian_convertable_v = (!(std::is_pointer_v<t> || std::is_const_v<t> || std::is_same_v<t, void>) &&
std::is_fundamental_v<t>) || std::is_enum_v<t>;

enum class endianness
{
	littleEndian,
	bigEndian
};
inline const endianness getCurrentEndianness()
{
	const int16_t twoByteValue = 0x1;
	return *((const int8_t*)&twoByteValue) ? endianness::littleEndian : endianness::bigEndian;
}

static const endianness currentEndianness = getCurrentEndianness();


inline void convertEndian(byte* ptr, cint& size)
{
	byte* endPtr = ptr + size - 1;
	while (endPtr > ptr)
	{
		std::swap(*ptr++, *endPtr--);
	}
}

template<typename t, typename = std::enable_if_t<is_endian_convertable_v<t>>>
inline void convertEndian(t& value)
{
	if constexpr (sizeof(t) > 1)
	{
		convertEndian((byte*)&value, sizeof(value));
	}
}
