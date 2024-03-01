#pragma once
#include "constants.h"
template<typename t>
struct nbtValue :nbtData
{
	t data = t();
	virtual inline bool serialize(const streamSerializer& s) override
	{
		static_assert(!std::is_pointer<t>::value, "no arrays");
		if (dataTag == nbtDataTag::tagUTF8String)
		{
			byte* ptr = (byte*)&data;
			s.serializeWStringAsString<int16_t>(*((std::wstring*)ptr));
		}
		else
		{
			if constexpr (is_endian_convertable_v<t>) {
				s.serialize(data);
			}
		}
		return true;
	}
	virtual inline void serialize(cbool& write, std::wstring& s) override
	{
		if (write)
		{
			if constexpr (std::is_integral<t>::value)
			{
				s = std::to_wstring(data);
			}
			else if constexpr (std::is_floating_point<t>::value)
			{
				s = std::to_wstring(data);
			}
			else if constexpr (std::is_same<t, std::wstring>::value)
			{
				s = data;
			}
			else
			{
				handleError(std::wstring(L"cant convert"));
			}
		}
		else
		{
			convertTo<t>(s, data);
		}
	}
	nbtValue(const std::wstring& name, const nbtDataTag& dataTag) :nbtData(name, dataTag), data(t()) {}
	virtual bool compare(const nbtData& other) const override;
};
template<typename t>
inline bool nbtValue<t>::compare(const nbtData& other) const
{
	return ((const nbtValue<t>&)other).data == data;
}