//#include <cstdlib>
#include <random>
#include <stdexcept>
#include "GlobalFunctions.h"
#include "math/mathfunctions.h"
#include "math/uuid.h"
#pragma once

template<typename t = int, typename = std::enable_if<std::is_integral<t>::value>>
inline t rand(std::mt19937& generator)
{
	std::uniform_int_distribution<t> distribution(0);
	return distribution(generator);
}
//CAN ALSO RETURN MAX
template<typename t = int, typename = std::enable_if<std::is_integral<t>::value>>
inline t rand(std::mt19937& generator, const t& max)
{
	std::uniform_int_distribution<t> distribution(0, max);
	return distribution(generator);
}
//CAN ALSO RETURN MAX
template<typename t = int, typename = std::enable_if<std::is_integral<t>::value>>
inline t rand(std::mt19937& generator, const t& min, const t& max)
{
	std::uniform_int_distribution<t> distribution(min, max);
	return distribution(generator);
}
//DOES RETURN MAX
//averageRollCount: the average amount of rolls to get a number out of this sequence
inline bool randChance(std::mt19937& generator, cint& averageRollCount)
{
	std::uniform_int_distribution<int> distribution(0, averageRollCount - 1);
	return distribution(generator) == 0;
}
template <typename t, typename = std::enable_if<std::is_integral<t>::value>>
inline t randIndex(std::mt19937& generator, const t& arraySize)
{
	std::uniform_int_distribution<t> distribution(0, arraySize - 1);
	return distribution(generator);
}
inline fsize_t randIndex(std::mt19937& generator, const std::vector<fp> weights, cfp& totalWeight)
{
	std::uniform_real_distribution<fp> distribution(0, totalWeight);
	fp value = distribution(generator);
	for (fsize_t i = 0; i < weights.size(); i++)
	{
		value -= weights[i];
		if (value <= 0)//<= because the distribution can also return the totalweight
		{
			return i;
		}
	}
	throw std::runtime_error("total weight too heigh");
}
inline int randIndex(std::mt19937& generator, const std::vector<int> weights, cint& totalWeight)
{
	std::uniform_int_distribution<int> distribution(0, totalWeight - 1);
	int value = distribution(generator);
	for (int i = 0; i < weights.size(); i++)
	{
		value -= weights[i];
		if (value < 0)
		{
			return i;
		}
	}
	throw std::runtime_error("total weight too heigh");
}

inline std::mt19937 getRandomFromSeed(const ull& seed)
{
	// Initialize the seed sequence with the full 64-bit seed
	std::seed_seq seedSeq{
	static_cast<uint>(seed),
	static_cast<uint>(seed >> 0x20)
	};
	// Initialize the Mersenne Twister random number generator with the seed sequence
	std::mt19937 mt(seedSeq);
	return mt;
}

inline int rand(cint& maxPlus1)
{
	return rand() % maxPlus1;
}
inline int rand(cint& min, cint& maxPlus1)
{
	return rand() % (maxPlus1 - min) + min;
}
inline fp randFp(std::mt19937& generator)
{
	std::uniform_real_distribution<fp> distribution(0, 1);
	return distribution(generator);
}
inline fp randFp(std::mt19937& generator, cfp& max)
{
	std::uniform_real_distribution<fp> distribution(0, max);
	return distribution(generator);
}
inline fp randFp(std::mt19937& generator, cfp& min, cfp& max)
{
	std::uniform_real_distribution<fp> distribution(min, max);
	return distribution(generator);
}

inline fp randFp()
{
	return (fp)rand() / RAND_MAX;
}
inline fp randFp(cfp& max)
{
	return randFp() * max;
}
inline fp randFp(cfp& min, cfp& max)
{
	return math::lerp(min, max, randFp());
}

inline int roundRandom(std::mt19937& generator, cfp& value)
{
	cint flooredValue = (int)floor(value);
	cfp fractionalValue = value - flooredValue;
	return randFp(generator) < fractionalValue ? flooredValue + 1 : flooredValue;
}

//returns in range [0,1]
inline fp getRandomDistanceFromCenter(std::mt19937& generator)
{
	return sqrt(randFp(generator));
}

inline uuid randomUUID(std::mt19937& generator) {
	uuid id = uuid();
	for (uint& n : id.idInts) {
		n = rand<uint>(generator);
	}
	return id;
}

std::wstring randomName(std::mt19937& generator);