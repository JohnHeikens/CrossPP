#include "fastlayer.h"

fp(*activationFunction)(cfp& x) = leakyReLu6;
fp(*activationFunctionDer)(cfp& x) = leakyReLu6_d;

fp(*activationFunctionEnd)(cfp& x) = sigmoid01;
fp(*activationFunctionDerEnd)(cfp& x) = sigmoid01_d;

fp(*checkWeights)(cfp& x) = checkClamp;

calculationLayer::calculationLayer(const fastArray<fastLayer*>& sourceLayers, cfsize_t& neuronCount) :
	calculationLayer(neuronCount)
{
	generateSourceLayersConnections(sourceLayers);
}

calculationLayer::calculationLayer(const fastArray<fastLayer*>& sourceLayers, cfsize_t& neuronCount, std::mt19937& randomToUse) :
	calculationLayer(sourceLayers, neuronCount)
{
	setRandomWeights(randomToUse);
}

calculationLayer::calculationLayer(cfsize_t& neuronCount) : fastLayer(neuronCount)
{
	totalSums = fastArray<fp>(values.size, true);
	biasWeights = fastArray<fp>(values.size, true);
	biasWeightMomentums = fastArray<fp>(values.size, true);
}

calculationLayer::calculationLayer(const std::vector<aiRawInputData*>& data) :fastLayer(data)
{
	totalSums = fastArray<fp>(values.size, true);
	biasWeights = fastArray<fp>(values.size, true);
	biasWeightMomentums = fastArray<fp>(values.size, true);
}

calculationLayer::~calculationLayer()
{
	for (auto it : sourceLayersConnections)
	{
		delete it;
	}
}

fsize_t calculationLayer::weightCountWithoutBias() const
{
	fsize_t result = 0;

	for (auto it : sourceLayersConnections)
	{
		result += it->sourceLayer->values.size;
	}
	return result;
}

void calculationLayer::setRandomWeights(std::mt19937& randomToUse) const
{
	for (auto connection : sourceLayersConnections)
	{
		for (fp& w : connection->weightMatrix)
		{
			w = reluInitialization(weightCountWithoutBias(),values.size,randomToUse);
		}
	}
}

/*void calculationLayer::setRandomWeights(const array2d<fp>& weightMatrixToReproduceFrom, cfp& alpha, std::mt19937& randomToUse) const
{
	cfp& deviation = sqrt((fp)2 / weightCountWithoutBias());

	fp* weightMatrixPtr = weightMatrix.begin();
	for (cfp& w : weightMatrixToReproduceFrom)
	{
		*weightMatrixPtr = deviation * alpha;
	}
}*/

void calculationLayer::generateSourceLayersConnections(const fastArray<fastLayer*>& sourceLayers)
{
	sourceLayersConnections = fastArray<sourceLayerConnection*>(sourceLayers.size);

	auto sourceLayerPtr = sourceLayers.begin();
	for (auto& connection : sourceLayersConnections)
	{
		connection = new sourceLayerConnection(*sourceLayerPtr, 
			array2d<fp>(vect2<size_t>(values.size, (*sourceLayerPtr)->values.size)), 
			array2d<fp>(vect2<size_t>(values.size, (*sourceLayerPtr)->values.size)));
		sourceLayerPtr++;
	}
}


/*calculationLayer::calculationLayer(const fastArray<fastLayer*>& sourceLayers, calculationLayer* const& reproduceFrom, cfp& alpha) :
	calculationLayer(sourceLayers,reproduceFrom->values.size)
{

	//generate weight matrix
	fp* reproduceFromPtr = reproduceFrom->weightMatrix.baseArray;
	const fp* const& endPtr = weightMatrix.baseArray + weightMatrix.size.volume();

	for (fp* ptr = weightMatrix.baseArray; ptr < endPtr; reproduceFromPtr++, ++ptr)
	{
		//tweak the values from reproduceFrom
		*ptr = checkWeights(*reproduceFromPtr + randFp(-alpha, alpha));
	}
	totalSums = fastArray<fp>(reproduceFrom->values.size, true);
}*/

void calculationLayer::Evaluate() const
{
	//multiply vector by the list

	//copy weight values, because the bottom layer does not have to be multiplied; the bias is always one
	std::copy(biasWeights.begin(), biasWeights.end(), totalSums.begin());

	for (const auto& it : sourceLayersConnections)
	{
		cfp* weightPtr = it->weightMatrix.begin();
		//loop weights + source neurons
		for (cfp& currentSourceLayerValue : it->sourceLayer->values)//add weights multiplied by the values from the source matrix
		{
			//loop current neurons
			for (fp& currentSum : totalSums)
			{
				currentSum += *weightPtr * currentSourceLayerValue;
				weightPtr++;
			}
		}
	}
}

fastLayer::fastLayer(const std::vector<aiRawInputData*>& data)
{
	fsize_t totalSize = 0;
	for (aiRawInputData* const& d : data)
	{
		totalSize += d->size;
	}
	values = fastArray<fp>(totalSize, true);

	fp* valuePtr = values.baseArray;

	for (aiRawInputData* const& d : data)
	{
		d->values = valuePtr;

		valuePtr += d->size;
	}
}
