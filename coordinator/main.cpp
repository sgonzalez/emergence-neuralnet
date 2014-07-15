/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include <iostream>
#include <string>
#include <unistd.h>

#include "host.h"

static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)> PERSISTENCE_FILE" << std::endl
        << "Options:\n"
        << "\t-h,--help\t\t\tShow this help message\n"
        << "\t-c,--child\t\t\tRun as a \"child\" process, i.e. with no REPL.\n"
        << "\t-C,--commands COMMANDS_FILE\tSpecify a command file to run on startup\n"
        << "\t-tk,--tmpkeep\t\t\tPrevent /tmp/emergence-neuralnet delete on termination"
        << std::endl;
}

Host *host;

void engage(std::string configFile, std::string commandsFile, bool child) {
    // initialize host
    host = new Host(realpath(configFile.c_str(), NULL));
    
    if (commandsFile != "") { // did the user supply a commands file
        char* commandspath = realpath(commandsFile.c_str(), NULL);
        if (access(commandspath, R_OK) != -1) { // make sure the commands file is accessible
            // run commands
            if (!host->runCommands(commandspath)) {
                std::cerr << "failed to run commands" << std::endl;
                exit(1);
            } else {
                // nn.saveNetwork();
                /// @todo decide wether we want to save after running commands by default
            }
        } else {
            std::cerr << "specified --commands file does not exist or does not have read permissions" << std::endl;
            exit(1);
        }
    }
    
    if (!child) {
        host->runWithREPL();
    }
}

bool keepTmp = false;

void cleanUp() {
    std::cout << std::endl << "Killing children..." << std::endl;
    host->killChildren();
    delete host;
    
    if (!keepTmp) {
        system("exec rm -rf /tmp/emergence-neuralnet");
        std::cout << "Deleting /tmp/emergence-neuralnet..." << std::endl;
    }
}

void intHandler(int sig) { exit(0); }

int main(int argc, char* argv[]) { // main almost exclusively does parameter processing
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }
    std::string configFile;
    std::string commandsFile = "";
    bool runningAsChild = false;
    for (int i = 1; i < argc; ++i) { // iterate over argument vector
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-c") || (arg == "--child")) {
            runningAsChild = true;
        } else if ((arg == "-tk") || (arg == "--tmpkeep")) {
            keepTmp = true;
        } else if ((arg == "-C") || (arg == "--commands")) {
            if (i + 1 < argc) { // make sure we aren't at the end of argv
                commandsFile = argv[++i]; // oncrement 'i' so we don't get the argument as the next argv[i].
            } else {
                std::cerr << "--commands option requires one argument." << std::endl;
                return 1;
            }  
        } else {
        // sources.push_back(argv[i]);
            configFile = argv[i];
        }
    }
    
    system("exec mkdir /tmp/emergence-neuralnet");
    
    atexit(cleanUp);
    signal(SIGINT, intHandler);

    engage(configFile, commandsFile, runningAsChild);

    return 0;
}