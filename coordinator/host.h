/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <unistd.h>

struct Child {
    std::string invocation;
    Child(std::string invocation) : invocation(invocation) {}
};

/// Host coordinates various child processes and vends command functionality, this is the main class. Only one instance of this should be running within the program.
class Host {
    std::map<std::string, Child> children;
    
    char *configpath;
    
    void readConfigFile(); ///< read in the configuration from an existing file that is accessible
    
    void newChild(std::string name, std::string invocation); ///< adds a new child to to be managed, referenced by name, called by invocation
    
public:
    Host(char *configpath);
    
    void runWithREPL();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
    
    void printSummary(std::string prefix);
    void printStats(std::string prefix);
    
    void saveConfiguration(); ///< saves the system's configuration
};