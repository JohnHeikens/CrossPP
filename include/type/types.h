#pragma once
#include <tuple>

template<size_t typeIndex, typename... types> using nthTypeOf =
typename std::tuple_element<typeIndex, std::tuple<types...>>::type;

template<typename... types> using firstTypeOf = nthTypeOf<0, types...>;