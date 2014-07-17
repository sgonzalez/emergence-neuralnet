/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

/// ChildHost manages the child, this is the main class. Only one instance of this should be running within the program.
class ChildHost {    
    std::map<std::string, std::map<std::string, std::string>> inputMappings;
    std::string outputFile;
    
    std::vector<std::string> inputNames, outputNames;
        
    void addInputMapping(std::string outputfilename, std::string outputname, std::string inputname); ///< maps an output from an XPC file to an input
    
    void runAsChildInterruptHandler();
    void runCoordinatorCommand();
    void update(); ///< this is where the real magic happens (PUT YOUR CODE IN HERE)
public:
    ChildHost();
    
    void runWithREPL();
    void runAsChild();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
};