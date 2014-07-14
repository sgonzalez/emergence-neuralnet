/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

#include "neuralnet.h"

/// NeuralHost manages the multi-layer perceptron (NeuralNet instance), this is the main class. Only one instance of this should be running within the program.
class NeuralHost {
    NeuralNet neuralnet;
    
    char *structurepath;
    char *weightspath;
    
    void readStructureFile(); ///< read in the structure from an existing file that is accessible
    void readWeightsFile(); ///< read in the weights from an existing file that is accessible, must be called AFTER readStructureFile()
public:
    NeuralHost(char *structurepath, char *weightspath);
    
    void runWithREPL();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
    
    void printSummary(std::string prefix);
    void printStats(std::string prefix);
    
    void saveNetwork(); ///< saves the network structure and weights to structurepath and weightspath, respectively
    
    // POTENTIALLY IMPLEMENT THESE TWO LATER
    // void loadNetworkState(char *statefile);
    // void saveNetworkState(char *statefile);
};