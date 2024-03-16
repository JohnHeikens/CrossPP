#pragma once
//#include <windows.h>
template <typename t>
inline bool convertTo(const std::wstring& s, t& value)
{
	if constexpr (std::is_same<t, std::wstring>::value)
	{
		csize_t& start = (s.length() && s[0] == L'\"') ? 1 : 0;
		csize_t& end = (s.length() && s[s.length() - 1] == L'\"') ? (s.length() - 1) : s.length();
		value = s.substr(start, end - start);
		return true;
	}
	else
	{
		if (s.size())
		{
			if constexpr (std::is_integral<t>::value)
			{
				long l;
				if (convertToLong(s, l))
				{
					value = (t)l;
					return true;
				}
			}
			else if constexpr (std::is_floating_point<t>::value)
			{
				fp d;
				if (convertToDouble(s, d))
				{
					value = (t)d;
					return true;
				}
			}
		}
	}
	return false;
}
template<typename tFrom, typename tTo>
inline tTo convertAnyType(const tFrom& value)
{
	if constexpr (std::is_arithmetic<tFrom>::value && std::is_arithmetic<tTo>::value)
	{
		return (tTo)value;
	}
	else if constexpr (std::is_same<tFrom, tTo>::value)
	{
		return value;
	}
	else if constexpr (std::is_same<tTo, std::wstring>::value)
	{
		if constexpr (std::is_pointer<tFrom>::value)
		{
			std::wstringstream ss = std::wstringstream();
			ss << (const void* const)value;
			return ss.str();
		}
		else
		{
			return std::to_wstring(value);
		}
	}
	else
	{
		throw "can not convert this type";
	}
}
