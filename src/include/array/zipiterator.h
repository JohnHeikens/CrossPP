#pragma once
#include "GlobalFunctions.h"
#include "type/types.h"
#include "optimization/macrotricks.h"
#include <iterator>

#ifdef _DEBUG
#define iteratorInline constexpr inline
#else
#define iteratorInline constexpr __forceinline
#endif // DEBUG

template<typename... valueTypes>
struct zippedValue
{
	std::tuple<valueTypes&...> values = std::tuple<valueTypes&...>();

	iteratorInline zippedValue(const std::tuple<valueTypes&...>& values) : values(values) {}
	//zippedValue(valueTypes&... values) : values(std::make_tuple(values...)) {}
	iteratorInline zippedValue(valueTypes&... values) : values(std::tie(values...)) {}

	iteratorInline zippedValue() :
		values()
	{

	}

	template<size_t index>
	iteratorInline nthTypeOf<index, valueTypes...>& val()
	{
		return std::get<index>(values);
	}

	iteratorInline void swap(zippedValue& with)
	{
		std::swap(values, with.values);
	}

	addAssignmentOperator(zippedValue, iteratorInline)

};

template<typename... valueTypes>
iteratorInline zippedValue<valueTypes...> zipValues(valueTypes&... values)
{
	return zippedValue(values...);
}

template<typename... iteratorTypes>
struct zipIterator
{
	std::tuple<iteratorTypes...> iterators = std::tuple<iteratorTypes...>();

	using firstIteratorType = std::tuple_element_t<0, std::tuple<iteratorTypes...>>;

	firstIteratorType end = firstIteratorType();


	iteratorInline zipIterator(const firstIteratorType& end, const iteratorTypes&... iterators) :
		iterators(std::make_tuple(iterators...)),
		end(end)
	{

	}

	addDefaultConstructors(zipIterator, iteratorInline)

		iteratorInline void swap(zipIterator& with)
	{
		std::swap(iterators, with.iterators);
		std::swap(end, with.end);
	}


	addAssignmentOperator(zipIterator, iteratorInline)


		// ++prefix operator
		iteratorInline zipIterator& operator++()
	{
		//https://stackoverflow.com/questions/64398212/incrementing-every-value-in-a-parameter-pack
		std::apply([](auto&&... a) {(++a, ...); }, iterators);
		return *this;
	}
	// --prefix operator
	iteratorInline zipIterator& operator--()
	{
		std::apply([](auto&&... a) {(--a, ...); }, iterators);
		return *this;
	}

	// postfix++ operator
	iteratorInline zipIterator operator++(int)
	{
		auto result = *this;
		++(*this);
		return result;
	}
	// postfix-- operator
	iteratorInline zipIterator operator--(int)
	{
		auto result = *this;
		--(*this);
		return result;
	}
	iteratorInline bool operator==(const zipIterator& other) const
	{
		return (other.iterators == iterators) && (other.end == end);
	}
	iteratorInline bool operator!=(const zipIterator& other) const
	{
		return !operator==(other);
	}
	iteratorInline decltype(auto) operator*()
	{
		//using returnType = decltype(zipValues(
		//	std::apply([](auto&& a) {return *a; }, iterators)
		//));

		return std::apply([](auto&&... a)
			{
				return zipValues((*a)...);
			}, iterators);
	}
};

template<typename... iteratorTypes>
iteratorInline decltype(auto) zipIterators(const nthTypeOf<0, iteratorTypes...>& firstEnd, const std::tuple<iteratorTypes...>& iterators)
{
	return zipIterator<iteratorTypes...>(firstEnd, iterators);
}

template<typename... iteratorTypes>
iteratorInline decltype(auto) zipIterators(const nthTypeOf<0, iteratorTypes...>& firstEnd, const iteratorTypes&... iterators)
{
	return zipIterator<iteratorTypes...>(firstEnd, iterators...);
}


template<typename... containerTypes>
struct zipIteratorContainer
{
	std::tuple<containerTypes&...> containers = std::tuple<containerTypes&...>();

	iteratorInline zipIteratorContainer(const std::tuple<containerTypes&...>& containers) :
		containers(containers)
	{
	}

	iteratorInline zipIteratorContainer(containerTypes&... containers) : containers(std::tie(containers...)) {}


	addDefaultConstructors(zipIteratorContainer, iteratorInline)

		iteratorInline void swap(zipIteratorContainer& with)
	{
		std::swap(containers, with.containers);
	}

	addAssignmentOperator(zipIteratorContainer, iteratorInline)

		iteratorInline decltype(auto) begin()
	{
		return std::apply([this](auto&&... a)
			{
				return zipIterators(std::end(std::get<0>(containers)), (std::begin(a))...);
			}, containers);
	}

	iteratorInline decltype(auto) end()
	{
		return std::apply([this](auto&&... a)
			{
				return zipIterators(std::end(std::get<0>(containers)), (std::end(a))...);
			}, containers);
	}

};

//todo: template overloading
template<typename... containerTypes,
	typename std::enable_if<(sizeof...(containerTypes) > 0), int>::type = 0>
	iteratorInline decltype(auto) zipTuple(const std::tuple<containerTypes&...>& toZip)
{
	//just call the constructor with whatever arguments there are
	return zipIteratorContainer(toZip);
}

template<typename... containerTypes,
	typename std::enable_if<(sizeof...(containerTypes) > 0), int>::type = 0>
	iteratorInline decltype(auto) zip(containerTypes&... toZip)
{
	//just call the constructor with whatever arguments there are
	return zipIteratorContainer(toZip...);
}