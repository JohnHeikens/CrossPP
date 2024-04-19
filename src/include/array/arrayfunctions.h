#pragma once

#include <vector>
#include <numeric>
#include "math/vectn.h"
#include <set>

template<typename First, typename ... T>
constexpr bool is_in(First &&first, T &&... t) {
    return ((first == t) || ...);
}

template<typename t, typename containerType>
constexpr t getSum(const containerType &values) {
    return std::accumulate(values.begin(), values.end(), t());
}

template<typename t>
constexpr t getAverage(const std::vector<t> &values) {
    return std::accumulate(values.begin(), values.end(), t()) / values.size();
}


template<typename listType, typename t>
inline bool insertSortedWithoutDoubles(listType &list, const t &value) {
    static_assert(!std::is_pointer<listType>::value, "pointers are not allowed");
    const auto index = std::lower_bound(list.begin(), list.end(), value);
    //insert sorted
    if ((index != list.end()) && (*index == value)) {
        return false;
    } else {
        list.insert(index, value);
        return true;
    }
}

//CAUTION! THIS FUNCTION IS VERY SLOW! PLEASE JUST SORT() AND UNIQUE() THE LIST AFTERWARDS
template<typename listType, typename t, typename compareFunction = std::less<t>>
inline bool insertSortedWithoutDoubles(listType &list, const t &value, compareFunction function) {
    static_assert(!std::is_pointer<listType>::value, "pointers are not allowed");
    const auto index = std::lower_bound(list.begin(), list.end(), value, function);
    //insert sorted
    if ((index != list.end()) && (*index == value)) {
        return false;
    } else {
        list.insert(index, value);
        return true;
    }
}


template<typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    // using std::stable_sort instead of std::sort
    // to avoid unnecessary index re-orderings
    // when v contains elements of equal values
    stable_sort(idx.begin(), idx.end(),
                [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

    return idx;
}

template<typename T, int arraySize>
constexpr int measureSize(T(&)[arraySize]) {
    return arraySize;
}

template<typename listType>
constexpr void deleteAllElements(const listType &list) {
    for (const auto *const &element: list) {
        delete element;
    }
}

template<typename listType, typename t>
constexpr bool arrayContains(const listType &list, const t &value) {
    return std::find(std::begin(list), std::end(list), value) != std::end(list);
}

template<typename listType, typename t>
constexpr bool addWithoutDupes(listType &list, const t &value) {
    if (!arrayContains(list, value)) {
        list.push_back(value);
        return true;
    }
    return false;
}


template<typename listType, typename t>
auto find(const listType &v, const t &value) {
    return std::find(std::begin(v), std::end(v), value);
}

template<typename listType, typename t>
size_t indexof(const listType &v, const t &value) {
    return std::distance(std::begin(v), find(v, value));
}

template<typename t, size_t n>
constexpr void fillAllElements(t(&arrayToFill)[n], const t &value) {
    std::fill(&arrayToFill[0], &arrayToFill[n], value);
}

template<typename t, size_t n>
constexpr void copyAllElements(const t(&arrayToCopy)[n], t *const &destination) {
    std::copy(&arrayToCopy[0], &arrayToCopy[n], destination);
}

template<typename sourceType, typename destType>
constexpr void copyAndCast(const sourceType* const& sourceIt, const sourceType* const& endIt, destType* const& destIt) {
    std::transform(sourceIt, endIt, destIt, [](const destType &value) {
        return (destType) value;
    });
}

template<typename checkFunction>
constexpr bool findInCircles(int maxRadius, checkFunction check, veci2 &result) {
    cint &maxRadiusSquared = maxRadius;

    std::multiset<std::pair<int, veci2>> positions = std::multiset<std::pair<int, veci2>>();
    for (int x = 0; x < maxRadius; x++) {
        cint &distanceXSquared = x * x;
        //cint& maxY = sqrt((fp)(maxRadiusSquared - distanceXSquared)); we need to find a faster way

        for (int y = 0; y <= x; y++) {
            cint &distanceSquaredPlusY = y * y + distanceXSquared;
            if (distanceSquaredPlusY < maxRadiusSquared) {
                positions.emplace(std::make_pair(distanceSquaredPlusY, veci2(x, y)));
            } else break;
        }
        for (auto it = positions.begin();
             it != positions.end() && it->first <= distanceXSquared; it = positions.erase(it)) {
            cveci2 &position = it->second;
            if (position.x == position.y) {
                if (position.x == 0) {
                    if (check(position)) {
                        result = position;
                        return true;
                    }
                } else {
                    cveci2 checkArray[4] = {
                            position,
                            cveci2(position.x, -position.y),
                            cveci2(-position.x, position.y),
                            cveci2(-position.x, -position.y)
                    };
                    for (cveci2 &pos: checkArray) {
                        if (check(pos)) {
                            result = pos;
                            return true;
                        }
                    }
                }
            } else {
                cveci2 checkArray[8] = {
                        position,
                        cveci2(position.x, -position.y),
                        cveci2(-position.x, position.y),
                        cveci2(-position.x, -position.y),
                        cveci2(position.y, -position.x),
                        cveci2(position.y, -position.x),
                        cveci2(-position.y, position.x),
                        cveci2(-position.y, -position.x),
                };
                for (cveci2 &pos: checkArray) {
                    if (check(pos)) {
                        result = pos;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}