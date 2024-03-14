#pragma once
#include "math/keyframe.h"
#include "aiinputdata.h"
#include "aiblueprint.h"
#include "fastlayer.h"
#include "math/graphics/texture.h"
#include <numeric>

constexpr fp costFunction(cfp& output, cfp& expectedOutput)
{
	return math::squared(expectedOutput - output);
}

constexpr fp costFunction_d(cfp& output, cfp& expectedOutput)
{
	return 2 * (expectedOutput - output);
}

static const color negativeColor = colorPalette::orange;
static const color positiveColor = colorPalette::blue;

static const transition<colorf> valueTransition = transition<colorf>(std::vector<keyFrame<colorf>>({
	keyFrame<colorf>(sigmoidMin, negativeColor),
	keyFrame<colorf>(sigmoidMid, colorPalette::white),
	keyFrame<colorf>(sigmoidMax, positiveColor)
	}));

struct neuralNetwork
{
public:
	aiBluePrint bluePrint = aiBluePrint();

	fastArray<calculationLayer*> hiddenLayers = fastArray<calculationLayer*>();

	fastLayer* inputLayer = nullptr;
	calculationLayer* outputLayer = nullptr;

	//the values of the last hidden layer, from the previous evaluation. used for erroring
	fastLayer* lastHiddenLayerValues = nullptr;

	fp lossFunctionAlpha = 0.0;

	//https://machinelearningmastery.com/learning-rate-for-deep-learning-neural-networks/


	fp momentumMultiplier = 0;//it is supposed to reduce jittering but generally stands in the way of the neural network learning anything.
	fp learningRate = 0.03;//value of 0 to 1, 0 = not learning, 1 = forgetting everything of the last epoch (a bit too much maybe)
	fp regularizationRate = 0.001;
	//if all weights from and to a neuron are smaller than this treshold, it's considered to be 'dead'.
	fp deadNeuronTreshold = 0.00001;

	//the impacts of each weight on the values of the last layer
	//+ means it dragged the value up

	//[hidden layer index][source layer index][current layer index][last hidden layer value index]

	fastArray<fastArray<array2d<fastArray<fp>>>> weightImpactsOnLastHiddenLayerValues = fastArray<fastArray<array2d<fastArray<fp>>>>();

	//[hidden layer index][current layer index][last hidden layer value index]
	fastArray<fastArray<fastArray<fp>>> biasWeightImpactsOnLastHiddenLayerValues = fastArray<fastArray<fastArray<fp>>>();

	//regression: the last hidden layer connects to the first in an infinite loop



	//0: least recent
	//size() - 1: most recent

	neuralNetwork(const aiBluePrint& bluePrint, fastLayer* inputLayer, calculationLayer* outputLayer);
	neuralNetwork(const aiBluePrint& bluePrint, fastLayer* inputLayer, calculationLayer* outputLayer, std::mt19937& randomToUse);
	//neuralNetwork(const neuralNetwork& replicateFrom, cfp& alpha, fastLayer* inputLayer, calculationLayer* outputLayer, std::mt19937& randomToUse);

	//https://www.youtube.com/watch?v=BmNKbnF69eY
	static fp calculateLoss(cfp& difference, cfp& alpha);

	neuralNetwork();

	~neuralNetwork();

	void evaluate();

	void reviveDeadNeurons(std::mt19937& randomToUse);

	void randomizeLayers(std::mt19937& randomToUse);

	//advised values :
	//alpha : 10 e-5 to 0.1 regularization rate : 0.001 to 0.01
	fp error(fp* expectedOutput);

	template<typename arrayType, typename accumulatedArrayType>
	inline accumulatedArrayType accumulateTweaks(const arrayType& listToSum, const fastArray<fp>& tweakArray)
	{
		const accumulatedArrayType& biasTweaks = accumulatedArrayType(listToSum.size);

		auto sumListIt = listToSum.begin();

		for (auto& it : biasTweaks)
		{
			//the tweaks each output layer value needs to be more correct next time
			const auto& tweaks = tweakArray * (*sumListIt);

			//add all tweaks together to get the sum
			it = std::accumulate(tweaks.begin(), tweaks.end(), (fp)0);

			sumListIt++;
		}
		return biasTweaks;
	}

	//positive = blue, negative red.
	inline color getColor(cfp& weight)
	{
		return weight > 0 ?
			color(colorPalette::blue, byte(color::multiplyByMaxValue(math::minimum(weight, (fp)1.0)))) :
			color(colorPalette::red, byte(color::multiplyByMaxValue(math::minimum(-weight, (fp)1.0))));
	}

	const fastLayer* getLayerToRender(cfsize_t& layerIndex) const
	{
		return (layerIndex == 0) ?
			inputLayer :
			((layerIndex == (hiddenLayers.size + 1)) ?
				outputLayer :
				hiddenLayers[layerIndex - 1]);
	}

	//layer 0 : input layer
	//layer 1 - [hiddenlayercount] : hidden layer
	//layer [hiddenlayercount] + 1 : output layer
	//returns a vec2 ranging from 0 to 1
	inline vec2 getRelativeNeuronPos(cfsize_t& layerIndex, cfsize_t& neuronIndex) const
	{
		cvect2<size_t>& divisor = cvect2<size_t>((bluePrint.hiddenNeuronCounts.size + 1), 
			(bluePrint.regression && (layerIndex == 0) && bluePrint.hiddenNeuronCounts.size) ? inputLayer->values.size + bluePrint.hiddenNeuronCounts[bluePrint.hiddenNeuronCounts.size - 1] :
			(getLayerToRender(layerIndex)->values.size - 1));

		return vec2(divisor.x() ? 
			(layerIndex / (fp)divisor.x()) : 0, 
			divisor.y() ?
			(neuronIndex / (fp)divisor.y()) : 0);
	}

	void renderBlobs(const texture& renderTarget, crectangle2& rect, cfp& blobSize) const;
};
