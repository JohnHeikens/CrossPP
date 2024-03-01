#pragma once
#include "SimplexNoise.h"//base noise
#include "interface/idestructable.h"
#include "mattnxn.h"

typedef unsigned int uint;
//#include <glad/glad.h>//typedefs
struct layerNoiseSimplex : IDestructable
{
public:
	layerNoiseSimplex(std::mt19937& randomToUse, const std::vector<fp>& octaveWeights, cfp noiseScale, crectangle1& outputRange = crectangle1(cvec1(0), cvec1(1)));
	layerNoiseSimplex(std::mt19937& randomToUse, const std::vector<fp>& octaveWeights, const std::vector<fp>& noiseScales, crectangle1& outputRange = crectangle1(cvec1(0), cvec1(1)));
	layerNoiseSimplex(std::mt19937& randomToUse, const std::vector<fp>& octaveWeights, const std::vector<vec3>& noiseScales, crectangle1& outputRange = crectangle1(cvec1(0), cvec1(1)));
	layerNoiseSimplex(std::mt19937& randomToUse, const std::vector<fp>& octaveWeights, crectangle1& outputRange = crectangle1(cvec1(0), cvec1(1)));

	simplexNoise* baseNoise = nullptr;
	std::vector<fp> octaveWeights = std::vector<fp>();
	std::vector<vec3> octaveFrequencies = std::vector<vec3>();//the multiplier for the input coordinates
	fp OutputPlus = 0;//the addition to the output to map it between min and max

	template<int dimensions>
	inline fp evaluate(const vecn<dimensions>& position) const
	{
		fp val = OutputPlus;
		for (int i = 0; i < octaveWeights.size(); i++)
		{
			val += octaveWeights[i] * baseNoise->evaluate<dimensions>(position * octaveFrequencies[i]);
		}
		return val;
	}
	virtual ~layerNoiseSimplex() override;
};