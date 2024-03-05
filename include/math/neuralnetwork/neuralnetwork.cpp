#include "neuralnetwork.h"
#include "math/graphics/brush/brushes.h"

neuralNetwork::neuralNetwork(const aiBluePrint& bluePrint, fastLayer* inputLayer, calculationLayer* outputLayer) :bluePrint(bluePrint), inputLayer(inputLayer), outputLayer(outputLayer)
{
	hiddenLayers = fastArray<calculationLayer*>(bluePrint.hiddenNeuronCounts.size, true);

	fastArray<fastLayer*> lastLayers;

	if (hiddenLayers.size && bluePrint.regression)
	{
		lastHiddenLayerValues = new fastLayer(bluePrint.hiddenNeuronCounts[bluePrint.hiddenNeuronCounts.size - 1]);
		lastLayers = { lastHiddenLayerValues, inputLayer };
	}
	else
	{
		lastLayers = { inputLayer };
	}

	if (hiddenLayers.size)
	{
		for (size_t i = 0; i < bluePrint.hiddenNeuronCounts.size; i++)
		{
			hiddenLayers[i] = new calculationLayer(lastLayers, bluePrint.hiddenNeuronCounts[i]);
			lastLayers = { hiddenLayers[i] };
		}
	}

	outputLayer->generateSourceLayersConnections(hiddenLayers.size ?
		fastArray<fastLayer*> { hiddenLayers[bluePrint.hiddenNeuronCounts.size - 1] } :
		fastArray<fastLayer*>{ inputLayer });

	if (bluePrint.regression)
	{
		weightImpactsOnLastHiddenLayerValues = fastArray <fastArray<array2d<fastArray<fp>>>>(bluePrint.hiddenNeuronCounts.size);
		biasWeightImpactsOnLastHiddenLayerValues = fastArray<fastArray<fastArray<fp>>>(bluePrint.hiddenNeuronCounts.size);

		if (bluePrint.hiddenNeuronCounts.size)
		{
			const fastLayer* const& lastHiddenlayer = hiddenLayers[bluePrint.hiddenNeuronCounts.size - 1];

			for (auto it : zip(weightImpactsOnLastHiddenLayerValues, biasWeightImpactsOnLastHiddenLayerValues, hiddenLayers))
			{
				const calculationLayer* const& currentHiddenLayer = it.val<2>();

				it.val<0>() = fastArray<array2d<fastArray<fp>>>(currentHiddenLayer->sourceLayersConnections.size);

				for (auto sourceLayerIt : zip(it.val<0>(), currentHiddenLayer->sourceLayersConnections))
				{
					sourceLayerIt.val<0>() = array2d<fastArray<fp>>(sourceLayerIt.val<1>()->weightMatrix.size);

					for (auto& weightIt : sourceLayerIt.val<0>())
					{
						weightIt = fastArray<fp>(lastHiddenlayer->values.size);
					}
				}

				it.val<1>() = fastArray<fastArray<fp>>(currentHiddenLayer->values.size);

				for (auto it : it.val<1>())
				{
					it = fastArray<fp>(lastHiddenlayer->values.size);
				}
			}
		}
	}
}

neuralNetwork::neuralNetwork(const aiBluePrint& bluePrint, fastLayer* inputLayer, calculationLayer* outputLayer, std::mt19937& randomToUse) :
	neuralNetwork(bluePrint, inputLayer, outputLayer)
{
	randomizeLayers(randomToUse);
}

/*neuralNetwork::neuralNetwork(const neuralNetwork& replicateFrom, cfp& alpha, fastLayer* inputLayer, calculationLayer* outputLayer, std::mt19937& randomToUse) :
	neuralNetwork(replicateFrom.bluePrint, inputLayer, outputLayer)
{
	calculationLayer** parentLayerPtr = replicateFrom.hiddenLayers.begin();
	for (calculationLayer* layer : hiddenLayers)
	{
		layer->setRandomWeights((*parentLayerPtr)->weightMatrix, alpha, randomToUse);
		++parentLayerPtr;
	}
	outputLayer->setRandomWeights(replicateFrom.outputLayer->weightMatrix, alpha, randomToUse);
}*/

fp neuralNetwork::calculateLoss(cfp& difference, cfp& alpha)
{
	cfp abs2alpha = abs(2 - alpha);
	//(abs(2 - alpha) / alpha) * pow(math::squared(difference) / abs(2 - alpha) + 1,alpha * 0.5) - 1);
	//alpha replaced by a and difference by x:
	//((abs(2 - a) / a) * pow((x * x) / abs(2 - a) + 1,a * 0.5) - 1);
	if (abs2alpha == 0) //will otherwise cause NAN errors
	{
		return math::squared(difference) / 2;
	}
	else
	{
		return (abs2alpha / alpha) *
			(
				pow(
					math::squared(difference) /
					abs2alpha
					+ 1,
					alpha * 0.5)
				- 1);
	}
}

neuralNetwork::neuralNetwork()
{
}

neuralNetwork::~neuralNetwork()
{
	for (fastLayer* const& layer : hiddenLayers)
	{
		delete layer;
	}

	delete inputLayer;
	delete outputLayer;
}


void neuralNetwork::evaluate()
{
	if (bluePrint.hiddenNeuronCounts.size && bluePrint.regression)
	{
		const calculationLayer* const& lastHiddenLayer = hiddenLayers[bluePrint.hiddenNeuronCounts.size - 1];
		std::copy(lastHiddenLayer->values.begin(), lastHiddenLayer->values.end(), lastHiddenLayerValues->values.begin());
	}

	for (calculationLayer* layer : hiddenLayers)
	{
		layer->Evaluate();
		layer->applySquashingFunction(reLu6);
	}

	outputLayer->Evaluate();
	outputLayer->applySquashingFunction(sigmoid01);
}

void neuralNetwork::reviveDeadNeurons(std::mt19937& randomToUse)
{
	//check if a neuron is dead
	//the output layer neurons can't be dead, because there is always a connection from the output layer to the output

	//when one dead neuron is revived, all dead neurons in it's source layer are automatically revived too.

	fastArray<fastArray<bool>> isAlive = fastArray<fastArray<bool>>(bluePrint.hiddenNeuronCounts.size);

	for (auto it : zip(isAlive, bluePrint.hiddenNeuronCounts))
	{
		it.val<0>() = fastArray<bool>(it.val<1>());
	}

	for (fsize_t layerIndex = 0; layerIndex < bluePrint.hiddenNeuronCounts.size + 1; layerIndex++)
	{
		cbool& isOutputLayer = layerIndex == bluePrint.hiddenNeuronCounts.size;
		calculationLayer* currentLayer = isOutputLayer ? outputLayer : hiddenLayers[layerIndex];

		cfsize_t& sourceLayerIndex = layerIndex ? (layerIndex - 1) : (bluePrint.hiddenNeuronCounts.size - 1);

		for (size_t connectionIndex = 0; connectionIndex < currentLayer->sourceLayersConnections.size; connectionIndex++)
		{
			sourceLayerConnection* currentConnection = currentLayer->sourceLayersConnections[connectionIndex];

			cbool& isInputLayer = currentConnection->sourceLayer == inputLayer;

			for (size_t valueIndex = 0; valueIndex < currentLayer->values.size; valueIndex++)
			{
				for (size_t sourceValueIndex = 0; sourceValueIndex < currentConnection->sourceLayer->values.size; sourceValueIndex++)
				{
					if (math::absolute(currentConnection->weightMatrix.getValueUnsafe(cvect2<size_t>(valueIndex, sourceValueIndex))) > deadNeuronTreshold)
					{
						if (!isOutputLayer)
						{
							isAlive[layerIndex][valueIndex] = true;
						}
						if (!isInputLayer)
						{
							isAlive[sourceLayerIndex][sourceValueIndex] = true;
						}
					}
				}
			}
		}
	}

	for (size_t layerIndex = 0; layerIndex < bluePrint.hiddenNeuronCounts.size; layerIndex++)
	{
		calculationLayer* currentLayer = hiddenLayers[layerIndex];
		for (size_t valueIndex = 0; valueIndex < bluePrint.hiddenNeuronCounts[layerIndex]; valueIndex++)
		{
			if (!isAlive[layerIndex][valueIndex])
			{
				//revive

				//set all weights from the source layer of that neuron to that neuron to a random weight

				for (sourceLayerConnection* connection : currentLayer->sourceLayersConnections)
				{
					for (size_t sourceLayerIndex = 0; sourceLayerIndex < connection->sourceLayer->values.size; sourceLayerIndex++)
					{
						connection->weightMatrix.setValueUnsafe(cvect2<size_t>(valueIndex, sourceLayerIndex), reluInitialization(currentLayer->weightCountWithoutBias(), currentLayer->values.size, randomToUse));
					}
				}
			}
		}
	}
}

void neuralNetwork::randomizeLayers(std::mt19937& randomToUse)
{
	for (calculationLayer* layer : hiddenLayers)
	{
		layer->setRandomWeights(randomToUse);
	}
	outputLayer->setRandomWeights(randomToUse);
}

//THE ALPHA HAS TO BE ABOVE ZERO, ELSE THEY GET DUMBER
fp neuralNetwork::error(fp* expectedOutput)
{
	//this will constantly be set to the impact of the values of the last layer

	auto valueImpactOnCurrentHiddenLayerValues = fastArray<fastArray<fp>>();

	if (bluePrint.hiddenNeuronCounts.size)
	{
		cfsize_t& lastHiddenLayerNeuronCount = bluePrint.hiddenNeuronCounts[bluePrint.hiddenNeuronCounts.size - 1];
		valueImpactOnCurrentHiddenLayerValues = fastArray<fastArray<fp>>(lastHiddenLayerNeuronCount);

		//identity matrix
		//impact from value 0 on value 0 is 1, on value x != 0 is 0
		size_t neuronIndex = 0;
		for (fastArray<fp>& neuronImpact : valueImpactOnCurrentHiddenLayerValues)
		{
			neuronImpact = fastArray<fp>(lastHiddenLayerNeuronCount);
			neuronImpact[neuronIndex] = 1;
			neuronIndex++;
		}
	}

	//[last layer value index][output layer value index]
	auto valueImpactOnOutputLayerValues = fastArray<fastArray<fp>>(outputLayer->values.size);


	//identity matrix
	//impact from value 0 on value 0 is 1, on value x != 0 is 0
	size_t neuronIndex = 0;
	for (fastArray<fp>& neuronImpact : valueImpactOnOutputLayerValues)
	{
		neuronImpact = fastArray<fp>(outputLayer->values.size);
		neuronImpact[neuronIndex] = 1;
		neuronIndex;
	}

	fastArray<array2d<fastArray<fp>>> lastLayerWeightImpactsOnOutputLayerValues = fastArray<array2d<fastArray<fp>>>();

	fastArray<fastArray<fp>> biasImpactsOnOutputLayerValues = fastArray<fastArray<fp>>();

	fastArray<fastArray<array2d<fastArray<fp>>>> weightImpactsOnHiddenLayerValues = fastArray<fastArray<array2d<fastArray<fp>>>>(bluePrint.hiddenNeuronCounts.size);
	fastArray<fastArray<fastArray<fp>>> biasImpactsOnHiddenLayerValues = fastArray<fastArray<fastArray<fp>>>(bluePrint.hiddenNeuronCounts.size);

	//update the weights impacts on the last hidden layers

	//from current to the past, to trace back the errors
	for (size_t layerIndex = bluePrint.hiddenNeuronCounts.size; ; layerIndex--)
	{
		cbool& isOutputLayer = layerIndex == bluePrint.hiddenNeuronCounts.size;

		const calculationLayer* const& currentLayer = isOutputLayer ? outputLayer : hiddenLayers[layerIndex];

		auto& weightImpactToCalculate = isOutputLayer ? lastLayerWeightImpactsOnOutputLayerValues : weightImpactsOnHiddenLayerValues[layerIndex];
		auto& biasImpactToCalculate = isOutputLayer ? biasImpactsOnOutputLayerValues : biasImpactsOnHiddenLayerValues[layerIndex];

		auto& valueImpactToCalculate = isOutputLayer ? valueImpactOnOutputLayerValues : valueImpactOnCurrentHiddenLayerValues;

		//pick the first source layer
		valueImpactToCalculate = currentLayer->calculateImpact(valueImpactToCalculate, weightImpactToCalculate, biasImpactToCalculate,
			isOutputLayer ? sigmoid01_d : reLu6_d)
			[0];

		if (layerIndex == 0) //first layer, cant apply impacts on the input values
		{
			break;
		}
	}
	//now valueimpactoncurrenthiddenlayervalues is the value impact from the last hidden layer values to current hidden layer values



	const auto& fromcurrentHiddenToOutputLayerImpact = array2d<fp>(valueImpactOnOutputLayerValues);




	//[last][current]
	const auto& fromLastToCurrentHiddenLayerImpact = array2d<fp>(valueImpactOnCurrentHiddenLayerValues);


	//multiply by matrix and add the current impacts

	auto hiddenLayerWeightImpactsOnOutputLayerValues = fastArray<fastArray<array2d<fastArray<fp>>>>(bluePrint.hiddenNeuronCounts.size);

	auto hiddenLayerBiasImpactsOnOutputLayerValues = fastArray<fastArray<fastArray<fp>>>(bluePrint.hiddenNeuronCounts.size);

	if (bluePrint.regression)
	{
		for (auto currentLayerIt : zip(weightImpactsOnHiddenLayerValues, hiddenLayerWeightImpactsOnOutputLayerValues, weightImpactsOnLastHiddenLayerValues))
		{
			currentLayerIt.val<1>() = fastArray<array2d<fastArray<fp>>>(currentLayerIt.val<0>().size);
			for (auto currentSourceLayerIt : zipTuple(currentLayerIt.values))
			{
				currentSourceLayerIt.val<1>() = array2d<fastArray<fp>>(currentSourceLayerIt.val<0>().size);

				for (auto currentWeightIt : zipTuple(currentSourceLayerIt.values))
				{
					currentWeightIt.val<0>() = currentWeightIt.val<0>() + fromLastToCurrentHiddenLayerImpact.multPointMatrix(currentWeightIt.val<2>());
					currentWeightIt.val<1>() = fromcurrentHiddenToOutputLayerImpact.multPointMatrix(currentWeightIt.val<0>());
				}
			}
		}

		for (auto currentLayerIt : zip(biasImpactsOnHiddenLayerValues, hiddenLayerBiasImpactsOnOutputLayerValues, biasWeightImpactsOnLastHiddenLayerValues))
		{
			currentLayerIt.val<1>() = fastArray<fastArray<fp>>(currentLayerIt.val<0>().size);

			for (auto currentWeightIt : zipTuple(currentLayerIt.values))
			{
				currentWeightIt.val<0>() = currentWeightIt.val<0>() + fromLastToCurrentHiddenLayerImpact.multPointMatrix(currentWeightIt.val<2>());
				currentWeightIt.val<1>() = fromcurrentHiddenToOutputLayerImpact.multPointMatrix(currentWeightIt.val<0>());
			}
		}
	}
	else
	{
		for (auto currentLayerIt : zip(weightImpactsOnHiddenLayerValues, hiddenLayerWeightImpactsOnOutputLayerValues))
		{
			currentLayerIt.val<1>() = fastArray<array2d<fastArray<fp>>>(currentLayerIt.val<0>().size);
			for (auto currentSourceLayerIt : zipTuple(currentLayerIt.values))
			{
				currentSourceLayerIt.val<1>() = array2d<fastArray<fp>>(currentSourceLayerIt.val<0>().size);

				for (auto currentWeightIt : zipTuple(currentSourceLayerIt.values))
				{
					currentWeightIt.val<1>() = fromcurrentHiddenToOutputLayerImpact.multPointMatrix(currentWeightIt.val<0>());
				}
			}
		}

		for (auto currentLayerIt : zip(biasImpactsOnHiddenLayerValues, hiddenLayerBiasImpactsOnOutputLayerValues))
		{
			currentLayerIt.val<1>() = fastArray<fastArray<fp>>(currentLayerIt.val<0>().size);

			for (auto currentWeightIt : zipTuple(currentLayerIt.values))
			{
				currentWeightIt.val<1>() = fromcurrentHiddenToOutputLayerImpact.multPointMatrix(currentWeightIt.val<0>());
			}
		}
	}



	//first the output layer
	fp totalError = 0;
	fastArray<fp> tweakArray = fastArray<fp>(outputLayer->values.size);
	for (size_t i = 0; i < outputLayer->values.size; i++)
	{
		cfp& cost = costFunction(outputLayer->values[i], expectedOutput[i]);
		//the impact of the output on the cost to DECREASE the error
		//derivative of math::squared(val - expectedOutput)
		//multiply by alpha to reduce the impact of this learning session
		tweakArray[i] = costFunction_d(outputLayer->values[i], expectedOutput[i]);
		totalError += cost;
	}

	//apply the weightImpacts
	for (auto layerIt : zip(hiddenLayers, hiddenLayerBiasImpactsOnOutputLayerValues, hiddenLayerWeightImpactsOnOutputLayerValues))
	{
		auto weightTweaks = fastArray<array2d<fp>>(layerIt.val<2>().size);

		for (auto sourceLayerIt : zip(weightTweaks, layerIt.val<2>()))
		{
			auto a = sourceLayerIt.val<1>();
			sourceLayerIt.val<0>() = accumulateTweaks<array2d<fastArray<fp>>, array2d<fp>>(a, tweakArray);
		}

		layerIt.val<0>()->nudgeWeights(
			accumulateTweaks<fastArray<fastArray<fp>>, fastArray<fp>>(layerIt.val<1>(), tweakArray),
			weightTweaks,
			momentumMultiplier,
			learningRate,
			regularizationRate,
			l2Regularization_d);
	}

	outputLayer->nudgeWeights(
		accumulateTweaks<fastArray<fastArray<fp>>, fastArray<fp>>(biasImpactsOnOutputLayerValues, tweakArray),
		{ accumulateTweaks<array2d<fastArray<fp>>, array2d<fp>>(lastLayerWeightImpactsOnOutputLayerValues[0], tweakArray) },
		momentumMultiplier,
		learningRate,
		regularizationRate,
		l2Regularization_d);

	weightImpactsOnLastHiddenLayerValues = weightImpactsOnHiddenLayerValues;
	biasWeightImpactsOnLastHiddenLayerValues = biasImpactsOnHiddenLayerValues;

	return totalError / outputLayer->values.size;
}

void neuralNetwork::renderBlobs(const texture& renderTarget, crectangle2& rect, cfp& blobSize) const
{
	crectangle2& innerRect = rect.expanded(blobSize * -0.5);

	//render the last hidden layer twice (once below the inputs, once below the last layer)
	for (fsize_t layerIndex = 0; layerIndex < bluePrint.hiddenNeuronCounts.size + 2; layerIndex++)
	{
		const fastLayer* currentLayer = getLayerToRender(layerIndex);
		for (fsize_t neuronIndex = 0; neuronIndex < currentLayer->values.size; neuronIndex++)
		{
			cvec2& absolutePosition = getAbsolutePosition(rect, getRelativeNeuronPos(layerIndex, neuronIndex));

			cfp& size = 1 + (defaultSigmoid(math::absolute(currentLayer->values[neuronIndex])) * blobSize);

			const color& colorToUse = colorPalette::blue;

			renderTarget.fillEllipseCentered(absolutePosition, cvec2(size), solidColorBrush(colorToUse));
		}
	}

	//render connections

	for (fsize_t layerIndex = 1; layerIndex < bluePrint.hiddenNeuronCounts.size + 2; layerIndex++)
	{
		const calculationLayer* const& currentLayer = (calculationLayer*)getLayerToRender(layerIndex);



		for (fsize_t sourceLayerIndex = 0; sourceLayerIndex < currentLayer->sourceLayersConnections.size; sourceLayerIndex++)
		{
			fsize_t sourceNeuronHeight = (sourceLayerIndex == currentLayer->sourceLayersConnections.size - 1) ? 0 : currentLayer->sourceLayersConnections[1]->sourceLayer->values.size;

			const fastLayer* const& sourceLayer = currentLayer->sourceLayersConnections[sourceLayerIndex]->sourceLayer;

			//render weight matrix

			for (fsize_t sourceNeuronIndex = 0; sourceNeuronIndex < sourceLayer->values.size; sourceNeuronIndex++, sourceNeuronHeight++)
			{
				cvec2& absolutePosition0 = getAbsolutePosition(rect, getRelativeNeuronPos(layerIndex - 1, sourceNeuronHeight));

				for (fsize_t currentNeuronIndex = 0; currentNeuronIndex < currentLayer->values.size; currentNeuronIndex++)
				{
					cvec2& absolutePosition1 = getAbsolutePosition(rect, getRelativeNeuronPos(layerIndex, currentNeuronIndex));

					cfp& value = currentLayer->sourceLayersConnections[sourceLayerIndex]->weightMatrix.getValueUnsafe(cvect2<size_t>(currentNeuronIndex, sourceNeuronIndex));

					//0 to 1
					cfp& part = defaultSigmoid(abs(value));


					const color& colorToUse = colorf((value < 0) ? negativeColor : positiveColor, part);

					renderTarget.fillLine(absolutePosition0, absolutePosition1, colorMixer<solidColorBrush, texture>(solidColorBrush(colorToUse), renderTarget));


				}
			}
		}
	}
}
