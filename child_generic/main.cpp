/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include <iostream>
#include <string>
#include <unistd.h>

#include "childhost.h"

static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)>" << std::endl
        << "Options:\n"
        << "\t-h,--help\t\t\tShow this help message\n"
        << "\t-c,--child\t\t\tRun as a child process, managed by coordinator. No REPL.\n"
        << "\t-C,--commands COMMANDS_FILE\tSpecify a command file to run on startup"
        << std::endl;
}


void engage(std::string commandsFile, bool child) {
    // initialize child
    ChildHost nn = ChildHost();
    
    if (commandsFile != "") { // did the user supply a commands file
        char* commandspath = realpath(commandsFile.c_str(), NULL);
        if (access(commandspath, R_OK) != -1) { // make sure the commands file is accessible
            // run commands
            if (!nn.runCommands(commandspath)) {
                std::cerr << "failed to run commands" << std::endl;
                exit(1);
            }
        } else {
            std::cerr << "specified --commands file does not exist or does not have read permissions" << std::endl;
            exit(1);
        }
    }
    
    if (!child) {
        nn.runWithREPL();
    } else {
        nn.runAsChild();
    }
}


int main(int argc, char* argv[]) { // main almost exclusively does parameter processing
    if (argc < 1) {
        show_usage(argv[0]);
        return 1;
    }
    std::string commandsFile = "";
    bool runningAsChild = false;
    for (int i = 1; i < argc; ++i) { // iterate over argument vector
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-c") || (arg == "--child")) {
            runningAsChild = true;
        } else if ((arg == "-C") || (arg == "--commands")) {
            if (i + 1 < argc) { // make sure we aren't at the end of argv
                commandsFile = argv[++i]; // oncrement 'i' so we don't get the argument as the next argv[i].
            } else {
                std::cerr << "--commands option requires one argument." << std::endl;
                return 1;
            }  
        }
    }

    engage(commandsFile, runningAsChild);

    return 0;
}