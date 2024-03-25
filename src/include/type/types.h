#pragma once
#include <tuple>
#include <stddef.h>
#include <cxxabi.h>  // needed for abi::__cxa_demangle
#include "array/wstring.h"

template<size_t typeIndex, typename... types> using nthTypeOf =
typename std::tuple_element<typeIndex, std::tuple<types...>>::type;

template<typename... types> using firstTypeOf = nthTypeOf<0, types...>;

inline std::wstring demangle(const char *abiName)
{
  int status;    
  char *ret = abi::__cxa_demangle(abiName, 0, 0, &status);  

  /* NOTE: must free() the returned char when done with it! */
  std::wstring retval = stringToWString(ret);
  free((void*)ret);
  //retval.reset( (char *)ret, [](char *mem) { if (mem) free((void*)mem); } );
  return retval;
}

template<typename t>
inline std::wstring getClassName(const t& instance)
{
	std::wstring name = demangle(typeid(instance).name());
  csize_t& spacePos = name.rfind(L' ');
	return spacePos == std::wstring::npos ? name :  name.substr(spacePos);
}