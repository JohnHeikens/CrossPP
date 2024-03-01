#pragma once
#include "array/fastarray.h"
//provides data from the neuralNetwork
struct aiBluePrint
{

	//regressionCount: the amount of neurons that store memory.
	//the amount of neurons that will be copied to the 'start'
	bool regression = false;

	//layerCount: amount of layers that calculate from input neurons, including first and last layer
	//layer 0: input layer
	//layer (layerCount - 1): outputlayer
	//neuroncount: array with the amount of neurons per layer. MEMORY NEURONS HAVE TO BE INCLUDED!
	fastArray<size_t> hiddenNeuronCounts = fastArray<size_t>();

	inline aiBluePrint(const fastArray<size_t>& hiddenNeuronCounts, cbool& regression) :
		hiddenNeuronCounts(hiddenNeuronCounts), regression(regression){  }
	inline aiBluePrint(){}
};