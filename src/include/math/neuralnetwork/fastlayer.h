#pragma once
#include "array/fastarray.h"
#include "math/mathfunctions.h"
#include "math/neuralnetwork/aiinputdata.h"
#include "array/rectiteratortn.h"
#include "array/zipiterator.h"
#include "array/arraynd.h"

static const fp reluInitialization(cfsize_t& previousLayerSize, cfsize_t& currentLayerSize, std::mt19937& randomToUse)
{
	//https://towardsdatascience.com/weight-initialization-techniques-in-neural-networks-26c649eb3b78
	//he initialization: works for reLu

	//https://stackoverflow.com/questions/64635958/understanding-min-and-max-values-of-randn-while-using-numpy

	cfp& max = sqrt((fp)2 / previousLayerSize);

	std::normal_distribution<fp> dis = std::normal_distribution<fp>();

	return dis(randomToUse) * max;

}

//https://stackoverflow.com/questions/10732027/fast-sigmoid-algorithm
//range[-1, 1]
constexpr fp defaultSigmoid(cfp& x) noexcept
{
	return x / (1 + math::absolute(x));
}

//range[0,1]
constexpr fp sigmoid01(cfp& x) noexcept
{
	//bring it from range [-1, 1] to [0, 1]
	return math::mapValue(defaultSigmoid(x), (fp)-1, (fp)1, (fp)0, (fp)1);
}
//https://www.derivative-calculator.net/
//derivative of sigmoid01
constexpr fp sigmoid01_d(cfp& x) noexcept
{
	return 1 / (2 * math::squared(math::absolute(x) + 1));
}
constexpr fp reLu(cfp& x) noexcept
{
	return x > 0 ? x : 0;
}
constexpr fp reLu_d(cfp& x) noexcept
{
	return x > 0 ? 1 : 0;
}

constexpr fp reLu6(cfp& x) noexcept
{
	return (x > 0) ? ((x < 6) ? x : 6) : 0;
}
constexpr fp reLu6_d(cfp& x) noexcept
{
	return ((x > 0) && (x < 6)) ? 1 : 0;
}

//the leaky relu mult depends on the amount of neurons that connect. more neurons = lower leaky relu mult, because of exploding gradients
cfp leakyRelumult = 0.01;
constexpr fp leakyReLu(cfp& x) noexcept
{
	return x > 0 ? x : x * leakyRelumult;
}
constexpr fp leakyReLu_d(cfp& x) noexcept
{
	return x > 0 ? 1 : leakyRelumult;
}
cfp treshold = 6;
constexpr fp leakyReLu6(cfp& x) noexcept
{
	return x > 0 ? x < treshold ? x : treshold + (x * leakyRelumult) : (x * leakyRelumult);
}
constexpr fp leakyReLu6_d(cfp& x) noexcept
{
	return x > 0 ? x < treshold ? 1 : leakyRelumult : leakyRelumult;
}
constexpr fp checkClamp(cfp& newValue) noexcept
{
	return math::clamp(newValue, (fp)-1.0, (fp)1.0);
}
constexpr fp linear(cfp& newValue) noexcept
{
	return newValue;
}

constexpr fp l1Regularization(cfp& weight) noexcept
{
	return math::absolute(weight);
}

constexpr fp l2Regularization(cfp& weight) noexcept
{
	return math::squared(weight);
}

constexpr fp l1Regularization_d(cfp& weight) noexcept
{
	return weight < 0 ? -1 : 1;
}

constexpr fp l2Regularization_d(cfp& weight) noexcept
{
	return 2 * weight;
}

extern fp(*activationFunction)(cfp& x);
extern fp(*activationFunctionDer)(cfp& x);

extern fp(*activationFunctionEnd)(cfp& x);
extern fp(*activationFunctionDerEnd)(cfp& x);

extern fp(*checkWeights)(cfp& x);

struct fastLayer : IDestructable
{
	fastArray<fp> values = fastArray<fp>();

	fastLayer(const std::vector<aiRawInputData*>& data);

	fastLayer(cfsize_t& neuronCount) : values(neuronCount) {}

	inline fastLayer(const fastLayer& other) : values(other.values)
	{

	}
	void swap(fastLayer& with)
	{
		std::swap(values, with.values);
	}

	inline fastLayer& operator=(fastLayer copy)
	{
		this->swap(copy);
		return *this;
	}
};

struct sourceLayerConnection
{
	fastLayer* sourceLayer = nullptr;
	//x: current layer, y: source layer
	//[y][x] -> [from][to]
	array2d<fp> weightMatrix = array2d<fp>();
	array2d<fp> weightMomentums = array2d<fp>();
	sourceLayerConnection() {}
	sourceLayerConnection(fastLayer* const& sourceLayer, const array2d<fp>& weightMatrix, const array2d<fp>& weightMomentums) :
		sourceLayer(sourceLayer), weightMatrix(weightMatrix), weightMomentums(weightMomentums) {}

	inline sourceLayerConnection(const sourceLayerConnection& other) : sourceLayer(other.sourceLayer),
		weightMatrix(other.weightMatrix),
		weightMomentums(other.weightMomentums)
	{
	}

	void swap(sourceLayerConnection& with)
	{
		std::swap(sourceLayer, with.sourceLayer);
		std::swap(weightMatrix, with.weightMatrix);
		std::swap(weightMomentums, with.weightMomentums);
	}

	inline sourceLayerConnection& operator=(sourceLayerConnection copy)
	{
		this->swap(copy);
		return *this;
	}
};

struct calculationLayer : fastLayer
{
	//x: neuron index
	fastArray<fp> totalSums = fastArray<fp>();
	fastArray<fp> biasWeights = fastArray<fp>();
	fastArray<fp> biasWeightMomentums = fastArray<fp>();

	//weights: 
	//x: current layer index
	//y: source layer index
	fastArray<sourceLayerConnection*> sourceLayersConnections = fastArray<sourceLayerConnection*>();


	//calculationLayer(const fastArray<fastLayer*>& sourceLayers, calculationLayer* const& reproduceFrom, cfp& alpha);
	calculationLayer(const fastArray<fastLayer*>& sourceLayers, cfsize_t& neuronCount);
	calculationLayer(const fastArray<fastLayer*>& sourceLayers, cfsize_t& neuronCount, std::mt19937& randomToUse);
	calculationLayer(cfsize_t& neuronCount);
	calculationLayer(const std::vector<aiRawInputData*>& data);

	~calculationLayer();

	fsize_t weightCountWithoutBias() const;

	void setRandomWeights(std::mt19937& randomToUse) const;
	//void setRandomWeights(const array2d<fp>& weightMatrixToReproduceFrom, cfp& alpha, std::mt19937& randomToUse) const;

	void generateSourceLayersConnections(const fastArray<fastLayer*>& sourceLayers);

	//DOES NOT APPLY THE SQUASHING FUNCTION
	void Evaluate() const;

	template<typename squashingFunctionType>
	void applySquashingFunction(squashingFunctionType activationFunction) const
	{
		//apply a squashing function
		fp* valueptr = values.baseArray;
		for (const fp& sum : totalSums)//first the last layer neurons then the hidden layer neurons
		{
			*valueptr = activationFunction(sum);
			valueptr++;
		}
	}

	inline static size_t getSourceLayerNeuronCountBias(const fastArray<fastLayer*>& sourceLayers)
	{
		size_t sourceLayerNeuronCountBias = 1;

		for (fastLayer* const& l : sourceLayers)
		{
			sourceLayerNeuronCountBias += l->values.size;
		}
		return sourceLayerNeuronCountBias;
	}

	//https://www.youtube.com/watch?v=tIeHLnjs5U8

	//[value index][output value index]
	//inputs: currentValuesImpact -> the impact the current values had on the error

	//weightsimpact -> the impact the weights had on the error

	//returns:
	//sourcevaluesimpact -> the impact the values of the source layer had on the error

	template<typename squashingFunctionDType>
	inline fastArray<fastArray<fastArray<fp>>> calculateImpact(
		const fastArray<fastArray<fp>>& currentValuesImpact,
		fastArray<array2d<fastArray<fp>>>& sourceLayerConnectionWeightImpacts,
		fastArray<fastArray<fp>>& biasWeightsImpact,

		squashingFunctionDType activationFunctionD) const
	{
		//do everything in reversed mode


		//the impact of each sum on the output value
		//[value index][output value index]
		fastArray<fastArray<fp>> totalSumsImpacts = fastArray<fastArray<fp>>(values.size);

		for (auto it : zip(totalSumsImpacts, totalSums, currentValuesImpact))
		{
			it.val<0>() = activationFunctionD(it.val<1>()) * (it.val<2>());
		}

		//the impact of each bias on the summed value is just one
		biasWeightsImpact = totalSumsImpacts;

		//the impact of each weight on the summed value is the source layer value that is multiplied by it
		sourceLayerConnectionWeightImpacts = fastArray<array2d<fastArray<fp>>>(sourceLayersConnections.size);

		//the impact of source layer value on the summed value is the sum of the weights that it gets multiplied by
		auto sourceValueImpacts = fastArray<fastArray<fastArray<fp>>>(sourceLayersConnections.size);

		for (auto sourceLayerIt : zip(sourceLayersConnections, sourceLayerConnectionWeightImpacts, sourceValueImpacts))
		{
			cfsize_t& outputLayerSize = currentValuesImpact[0].size;

			sourceLayerIt.val<1>() = array2d<fastArray<fp>>(sourceLayerIt.val<0>()->weightMatrix.size);
			sourceLayerIt.val<2>() = fastArray<fastArray<fp>>(sourceLayerIt.val<0>()->sourceLayer->values.size);

			auto it = sourceLayerIt.val<2>();

			std::fill(sourceLayerIt.val<1>().begin(), sourceLayerIt.val<1>().end(), fastArray<fp>(outputLayerSize));
			std::fill(sourceLayerIt.val<2>().begin(), sourceLayerIt.val<2>().end(), fastArray<fp>(outputLayerSize));

			for (fsize_t sourceLayerIndex = 0; sourceLayerIndex < sourceLayerIt.val<0>()->sourceLayer->values.size; ++sourceLayerIndex)
			{
				for (fsize_t currentLayerIndex = 0; currentLayerIndex < values.size; ++currentLayerIndex)
				{
					sourceLayerIt.val<1>()[sourceLayerIndex][currentLayerIndex] = totalSumsImpacts[currentLayerIndex] * sourceLayerIt.val<0>()->sourceLayer->values[sourceLayerIndex];
					sourceLayerIt.val<2>()[sourceLayerIndex] += totalSumsImpacts[currentLayerIndex] * sourceLayerIt.val<0>()->weightMatrix[sourceLayerIndex][currentLayerIndex];
				}
			}
		}

		return sourceValueImpacts;
	}

	//add impacts to weights
	template <typename regularizationFunctionDType>
	inline void nudgeWeights(const fastArray<fp>& biasTweaks, const fastArray<array2d<fp>>& weightTweaks, cfp& momentumMultiplier, cfp& learningRate, cfp& regularizationRate, regularizationFunctionDType regularizationFunction_d)
	{
		cfp& multipliedLearningRate = (1 - momentumMultiplier) * learningRate;

		cfp& multipliedRegularizationRate = multipliedLearningRate * regularizationRate;

		biasWeightMomentums += biasTweaks * multipliedLearningRate;

		//add regularization to weights


		biasWeights += biasWeightMomentums;

		if (multipliedRegularizationRate > 0)
		{
			for (auto biasIt : zip(biasWeightMomentums, biasWeights))
			{
				biasIt.val<0>() -= (regularizationFunction_d(biasIt.val<1>()) * multipliedRegularizationRate);
			}
		}

		biasWeightMomentums *= momentumMultiplier;

		for (auto layerIt : zip(sourceLayersConnections, weightTweaks))
		{
			layerIt.val<0>()->weightMomentums += layerIt.val<1>() * multipliedLearningRate;
			layerIt.val<0>()->weightMatrix += layerIt.val<0>()->weightMomentums;
			layerIt.val<0>()->weightMomentums *= momentumMultiplier;

			if (multipliedRegularizationRate > 0)
			{
				for (auto weightIt : zip(layerIt.val<0>()->weightMomentums, layerIt.val<0>()->weightMatrix))
				{
					weightIt.val<0>() -= (regularizationFunction_d(weightIt.val<1>()) * multipliedRegularizationRate);
				}
			}
		}
	}

	inline calculationLayer(const calculationLayer& other) : fastLayer(other),
		sourceLayersConnections(fastArray<sourceLayerConnection*>(other.sourceLayersConnections.size)),
		biasWeights(other.biasWeights),
		biasWeightMomentums(other.biasWeightMomentums),
		totalSums(other.totalSums)
	{
		auto copiedIt = sourceLayersConnections.begin();
		for (const auto& it : other.sourceLayersConnections)
		{
			*copiedIt = new sourceLayerConnection(*it);
			copiedIt++;
		}
	}

	void swap(calculationLayer& with)
	{
		fastLayer::swap(with);
		std::swap(sourceLayersConnections, with.sourceLayersConnections);
		std::swap(biasWeights, with.biasWeights);
		std::swap(biasWeightMomentums, with.biasWeightMomentums);
		std::swap(values, with.values);
		std::swap(totalSums, with.totalSums);
	}

	inline calculationLayer& operator=(calculationLayer copy)
	{
		this->swap(copy);
		return *this;
	}
};
