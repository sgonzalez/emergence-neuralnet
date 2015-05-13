/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "neuralnet.h"
#include "genetic.h"
#include "utils.h"

/// NeuralHost manages the multi-layer perceptron (NeuralNet instance), this is the main class. Only one instance of this should be running within the program.
class NeuralHost {
    NeuralNet neuralnet;
    
    char *structurepath;
    char *weightspath;
    
    std::map<std::string, std::map<std::string, std::string>> inputMappings;
    std::string outputFile;
    
    void readStructureFile(); ///< read in the structure from an existing file that is accessible
    void readWeightsFile(); ///< read in the weights from an existing file that is accessible, must be called AFTER readStructureFile()
    
	void trainNetwork(std::string trainname, std::string testname, int popsize, int generations);

    void addInputMapping(std::string outputfilename, std::string outputname, std::string inputname); ///< maps an output from an XPC file to an input
    
    void runAsChildInterruptHandler();
    void runCoordinatorCommand();
    void update();
public:
    NeuralHost(char *structurepath, char *weightspath);
    
    void runWithREPL();
    void runAsChild();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
    
    void printSummary(std::string prefix);
    void printStats(std::string prefix);
    void timePropagation();
    
    void saveNetwork(); ///< saves the network structure and weights to structurepath and weightspath, respectively
};