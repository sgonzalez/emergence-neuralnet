/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <math.h>

struct Neuron {
	int numInputs; ///< the number of inputs/synapses into the neuron (+ the bias)
	std::vector<double> weights; ///< the corresponding weights for each input (+ the bias at the end)
    Neuron(int numberInputs);
    void addInput(); ///< adds a new input weight to the neuron
    void removeInput(int index); ///< removes an input weight from the neuron
    void removeInput(); ///< removes the last input weight (not the bias) from the neuron
};

struct NeuronLayer {
	int numNeurons; ///< the number of neurons in this layer
    int numInputsPerNeuron; ///< the number of inputs that each neuron has
	std::vector<Neuron> neurons; ///< the corresponding neurons
	NeuronLayer(int numberNeurons, int numberInputsPerNeuron);
	void addInput(); ///< adds a new input weight to every neuron in the layer
    void addNeuron(); ///< adds a new neuron to the layer, not responsible for forming downstream synapses
    void removeInput(int index); ///< removes an input weight from every neuron in the layer
    void removeInput(); ///< removes the last input weight (not the bias) from every neuron in the layer
    void removeNeuron(int index); ///< removes a neuron from the layer, not responsible for destroying downstream synapses
    void removeNeuron(); ///< removes the last neuron from the layer, not responsible for destroying downstream synapses
};

/// NeuralNet is the neural network itself
class NeuralNet {
    static const double activationResponse;
    static const double biasCoefficient;
    
    int numInputs;
    int numOutputs;
    int numHiddenLayers;
    
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<NeuronLayer> layers;
public:
    NeuralNet();
    
    std::vector<std::string> getInputs() const;
    std::vector<std::string> getOutputs() const;
    std::vector<NeuronLayer> getLayers() const;
    
    void addInput(std::string name);
    void addOutput(std::string name);
    void removeInput(std::string name);
    void removeOutput(std::string name);
    void addNeurons(int layer, int quantity);
    void removeNeurons(int layer, int quantity);
    
    void addLayerBeforeOutputLayer(int numNeurons, int numInputsPerNeuron); ///< inserts a layer before the output layer
    void addLayer(int layerIndex, int numNeurons);
    void removeLayer(int layerIndex);
    
    void randomizeWeights(); ///< rerandomizes all the weights in the network
    void zeroWeights(); ///< zeroes all the weights in the network
    std::vector<double> getWeights() const; ///< returns the neural network's weights by layer
    int getNumberOfWeights() const; ///< returns the total number of weights in the network
    void setWeights(std::vector<double> &weights); ///< updates the network's weights with a new set
    
    std::vector<double> propagate(std::vector<double> &inputs); ///< propagates inputs through to find outputs
    
    double sigmoid(double activation, double response); ///< the sigmoid response curve
};