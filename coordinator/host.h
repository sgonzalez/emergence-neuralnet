/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

/// Host coordinates various child processes and vends command functionality, this is the main class. Only one instance of this should be running within the program.
class Host {
    
    char *configpath;
    
    void readConfigFile(); ///< read in the configuration from an existing file that is accessible
public:
    Host(char *configpath);
    
    void runWithREPL();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
    
    void printSummary(std::string prefix);
    void printStats(std::string prefix);
    
    void saveConfiguration(); ///< saves the system's configuration
};