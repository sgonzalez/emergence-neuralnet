///////////////////////////////////////////////////////////////
/// Copyright 2015 by Santiago Gonzalez <slgonzalez@me.com> ///
///////////////////////////////////////////////////////////////

#include "neuralnet.h"
#include "utils.h"


/////////////////////////
// Neuron

Neuron::Neuron(int numberInputs) {
    numInputs = numberInputs + 1; // +1 for an additional weight for the bias

	for (int i = 0; i < numInputs; i++) {
		// set up the weights with an initial random value
		weights.push_back(randomClamped());
	}
}

void Neuron::addInput() {
    numInputs++;
    weights.insert(weights.end() - 1, randomClamped()); // insert before the bias weight
}

void Neuron::removeInput(int index) {
    numInputs--;
    weights.erase(weights.begin() + index);
}

void Neuron::removeInput() {
    numInputs--;
    weights.erase(weights.end() - 2);
}

/////////////////////////
// Neuron Layer

NeuronLayer::NeuronLayer(int numberNeurons, int numberInputsPerNeuron) {
    numNeurons = numberNeurons;
    numInputsPerNeuron = numberInputsPerNeuron;
    for (int i = 0; i < numNeurons; i++) {
        neurons.push_back(Neuron(numberInputsPerNeuron));
    }
}

void NeuronLayer::addInput() {
    numInputsPerNeuron++;
    for (int i = 0; i < neurons.size(); i++) {
        neurons[i].addInput();
    }
}

void NeuronLayer::addNeuron() {
    numNeurons++;
    neurons.push_back(Neuron(numInputsPerNeuron));
}

void NeuronLayer::removeInput(int index) {
    numInputsPerNeuron--;
    for (int i = 0; i < neurons.size(); i++) neurons[i].removeInput(index);
}

void NeuronLayer::removeInput() {
    numInputsPerNeuron--;
    for (int i = 0; i < neurons.size(); i++) neurons[i].removeInput();
}

void NeuronLayer::removeNeuron(int index) {
    numNeurons--;
    neurons.erase(neurons.begin() + index);
}

void NeuronLayer::removeNeuron() {
    numNeurons--;
    neurons.erase(neurons.end() - 1);
}

/////////////////////////
// Neural Network

const double NeuralNet::activationResponse = 1;
const double NeuralNet::biasCoefficient = -1;

NeuralNet::NeuralNet() {
    numInputs = 0;
    numOutputs = 0;
    numHiddenLayers = 0;
    
    // create empty output layer
    layers.push_back(NeuronLayer(0, 0));
}

std::vector<std::string> NeuralNet::getInputs() const { return inputs; }
std::vector<std::string> NeuralNet::getOutputs() const { return outputs; }
std::vector<NeuronLayer> NeuralNet::getLayers() const { return layers; }

void NeuralNet::addInput(std::string name) {
    numInputs++;
    inputs.push_back(name);
    layers[0].addInput();
}

void NeuralNet::addOutput(std::string name) {
    numOutputs++;
    outputs.push_back(name);
    layers.back().addNeuron();
}

void NeuralNet::removeInput(std::string name) {
    std::vector<std::string>::iterator position = std::find(inputs.begin(), inputs.end(), name);
    if (position != inputs.end()) { // make sure the element exists
        numInputs--;
        inputs.erase(position);
        int index = position - inputs.begin();
        layers[0].removeInput(index);
    }
}

void NeuralNet::removeOutput(std::string name) {
    std::vector<std::string>::iterator position = std::find(outputs.begin(), outputs.end(), name);
    if (position != outputs.end()) { // make sure the element exists
        numOutputs--;
        outputs.erase(position);
        int index = position - outputs.begin();
        layers.back().removeNeuron(index);
    }
}

void NeuralNet::addNeurons(int layer, int quantity) { // note: here, layer is 1-indexed relative to layers
    if (layer == layers.size()) {
        std::cerr << "Cannot add neurons to output layer using 'neuronadd'! Use 'outputadd' instead." << std::endl;
    } else if (layer == 0) {
        std::cerr << "Cannot add neurons to intput layer using 'neuronadd'! Use 'inputadd' instead." << std::endl;
    } else if (layer < 0 || layer > layers.size()) {
        std::cerr << "Layer " << layer << " is not a valid layer." << std::endl;
    } else { // layer is a hidden layer
        for (int i = 0; i < quantity; i++) {
            layers[layer-1].addNeuron();
            layers[layer].addInput(); // add downstream synapses
        }
    }
}

void NeuralNet::removeNeurons(int layer, int quantity) { // note: here, layer is 1-indexed relative to layers
    if (layer == layers.size()) {
        std::cerr << "Cannot remove neurons from output layer using 'neuronremove'! Use 'outputremove' instead." << std::endl;
    } else if (layer == 0) {
        std::cerr << "Cannot remove neurons from intput layer using 'neuronremove'! Use 'inputremove' instead." << std::endl;
    } else { // layer is a hidden layer
        for (int i = 0; i < quantity; i++) {
            layers[layer-1].removeNeuron();
            layers[layer].removeInput(); // remove downstream synapses
        }
    }
}


void NeuralNet::addLayerBeforeOutputLayer(int numNeurons, int numInputsPerNeuron) {
    layers.insert(layers.end() - 1, NeuronLayer(numNeurons, numInputsPerNeuron));
}

void NeuralNet::addLayer(int layerIndex, int numNeurons) { // note: here, layer is 1-indexed relative to layers
    int numInputsPerNeuron = layers[layerIndex - 1].numInputsPerNeuron; // adopt same number of inputs
    layers.insert(layers.begin() + layerIndex - 1, NeuronLayer(numNeurons, numInputsPerNeuron));
    
    for (int j = 0; j < layers[layerIndex - 1].numNeurons; ++j) { // iterate over new neurons
        if (j >= layers[layerIndex].numNeurons) break;
        layers[layerIndex - 1].neurons[j].weights = layers[layerIndex].neurons[j].weights; // adopt weights from layer that was here previously
    }
    
    for (int j = 0; j < layers[layerIndex].numNeurons; ++j) { // iterate over preexisting neurons
        double sum = 0;
        for (int k = 0; k < layers[layerIndex].neurons[j].numInputs - 1; ++k) { // iterate over weights (- bias)
            sum += layers[layerIndex].neurons[j].weights[k];
        }
        double bias = layers[layerIndex].neurons[j].weights[layers[layerIndex].neurons[j].numInputs - 1]; // save bias
        double averageWeight = sum / (layers[layerIndex].neurons[j].numInputs - 1); // calculate average weight (- bias)
        layers[layerIndex].neurons[j].numInputs = numNeurons + 1; // update number of downstream inputs (+ bias)
        layers[layerIndex].neurons[j].weights.clear(); // dump old weights
        for (int k = 0; k < layers[layerIndex].neurons[j].numInputs - 1; ++k) { // iterate over new number of weights (- bias)
            layers[layerIndex].neurons[j].weights.push_back(averageWeight); // add new weight, average of old weights
        }
        layers[layerIndex].neurons[j].weights.push_back(bias); // restore bias
    }
    layers[layerIndex].numInputsPerNeuron = numNeurons; // update preexisting layer number of inputs
}

void NeuralNet::removeLayer(int layerIndex) { // note: here, layer is 1-indexed relative to layers
    layers[layerIndex].numInputsPerNeuron = layers[layerIndex-1].numInputsPerNeuron; // adopt number of inputs
    
    for (int j = 0; j < layers[layerIndex].numNeurons; ++j) { // iterate over preexisting, downstream neurons
        float bias = layers[layerIndex].neurons[j].weights[layers[layerIndex].neurons[j].numInputs - 1];
        layers[layerIndex].neurons[j].numInputs = layers[layerIndex].numInputsPerNeuron; // update number of inputs
        if (j >= layers[layerIndex].numNeurons) {
            for (int i = 0; i < layers[layerIndex].neurons[j].numInputs; i++) // no weights to adopt, so make random
        		layers[layerIndex].neurons[j].weights.push_back(randomClamped());
        } else {
            layers[layerIndex].neurons[j].weights = layers[layerIndex-1].neurons[j].weights; // adopt weights from layer to be removed
        }
        layers[layerIndex].neurons[j].weights[layers[layerIndex].neurons[j].numInputs - 1] = bias; // don't adopt bias
    }
    
    layers.erase(layers.begin() + layerIndex - 1);
}


void NeuralNet::randomizeWeights() {
    for (int i = 0; i < numHiddenLayers + 1; ++i) { // iterate over layers
		for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
			for (int k = 0; k < layers[i].neurons[j].numInputs; ++k) { // iterate over weights
                layers[i].neurons[j].weights[k] = randomClamped();
			}		
		}
	}
}

void NeuralNet::zeroWeights() {
    for (int i = 0; i < numHiddenLayers + 1; ++i) { // iterate over layers
		for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
			for (int k = 0; k < layers[i].neurons[j].numInputs; ++k) { // iterate over weights
                layers[i].neurons[j].weights[k] = 0;
			}		
		}
	}
}

std::vector<double> NeuralNet::getWeights() const {
    std::vector<double> weights;
	for (int i = 0; i < numHiddenLayers + 1; ++i) { // iterate over layers
		for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
			for (int k = 0; k < layers[i].neurons[j].numInputs; ++k) { // iterate over weights
				weights.push_back(layers[i].neurons[j].weights[k]);
			}
		}
	}
	return weights;
}

int NeuralNet::getNumberOfWeights() const {
    int count = 0;
	for (int i = 0; i < numHiddenLayers + 1; ++i) { // iterate over layers
		for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
			for (int k = 0; k < layers[i].neurons[j].numInputs; ++k) { // iterate over weights
				count++;
			}		
		}
	}
	return count;
}

void NeuralNet::setWeights(std::vector<double> &weights) {
    int currentWeight = 0;
	for (int i = 0; i < numHiddenLayers + 1; ++i) { // iterate over layers
		for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
			for (int k = 0; k < layers[i].neurons[j].numInputs; ++k) { // iterate over weights
				layers[i].neurons[j].weights[k] = weights[currentWeight++];
			}
		}
	}
}

std::vector<double> NeuralNet::propagate(std::vector<double> &inputs) {
    std::vector<double> outputs; // the resultant outputs from each layer

    // error check number of inputs
    if (inputs.size() != numInputs) {
        std::cerr << "Incorrect number of inputs! Expected " << numInputs << ", received " << inputs.size() << std::endl;
        return outputs; // return empty vector
    }

    // iterate over layers
    for (int i = 0; i < numHiddenLayers + 1; ++i) {
        
        if ( i > 0 ) // if not the input layer
            inputs = outputs;

        outputs.clear();

        int cWeight = 0; // the current weight

        // for each neuron sum the (inputs * corresponding weights). Throw the total at our sigmoid function to get the output.
        for (int j = 0; j < layers[i].numNeurons; ++j) { // iterate over neurons
            double netInput = 0;
            int	lclnumInputs = layers[i].neurons[j].numInputs;

            for (int k = 0; k < lclnumInputs - 1; ++k) { // iterate over weights
                netInput += layers[i].neurons[j].weights[k] * inputs[cWeight++]; // evaluate the linear combination
            }

            netInput += layers[i].neurons[j].weights[lclnumInputs-1] * biasCoefficient; // add in the bias

            outputs.push_back(sigmoid(netInput, activationResponse)); // store output and pass activation through sigmoid

            cWeight = 0;
        }
    }

    return outputs;
}

double NeuralNet::sigmoid(double activation, double response) {
    return 1 / (1 + exp(-activation / response));
}


