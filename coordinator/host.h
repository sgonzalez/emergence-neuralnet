/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

struct Child {
    std::string invocation;
    std::vector<std::string> argv;
    Child(std::string invocation, std::vector<std::string> argv) : invocation(invocation), argv(argv) {}
};

/// Host coordinates various child processes and vends command functionality, this is the main class. Only one instance of this should be running within the program.
class Host {
    std::map<std::string, Child> children;
    
    std::map<std::string, pid_t> pids;
    bool started;
    
    char *configpath;
    
    float targetUpdateInterval; ///< in seconds
    
    void readConfigFile(); ///< read in the configuration from an existing file that is accessible
    
    void addChild(std::string name, std::string invocation); ///< adds a new child to to be managed, referenced by name, called by invocation
    void removeChild(std::string name);
    
    void updateChildren();
    void childRunCommand(std::string name, std::string command);
    
public:
    Host(char *configpath);
    
    void runWithREPL();
    void start();
    void run();
    
    void killChildren();
    
    bool runCommands(char *filepath); ///< sequentially run the commands in the provided file
    bool runCommand(std::string command);
    
    void printSummary(std::string prefix);
    void printStats(std::string prefix);
    
    void saveConfiguration(); ///< saves the system's configuration
};