///////////////////////////////////////////////////////////////
/// Copyright 2015 by Santiago Gonzalez <slgonzalez@me.com> ///
///////////////////////////////////////////////////////////////

#include "ChildHost.h"

ChildHost::ChildHost() {
    srand(time(NULL)); // seed the prng
    
    inputNames.push_back("x");
    inputNames.push_back("y");
    inputNames.push_back("z");
    outputNames.push_back("nx");
    outputNames.push_back("ny");
    outputNames.push_back("nz");
}

void ChildHost::update() {
    // Configure default inputs
    std::vector<double> inputs;
    int size = inputNames.size();
    for (int i = 0; i < size; i++) {
        inputs.push_back(0);
    }
    
    // Read inputs
    for (std::pair<std::string, std::map<std::string, std::string>> fileentry : inputMappings) {
        // read input file
        std::map<std::string, double> fileoutputs;
        std::ifstream inputfile(fileentry.first);
        std::string line;
        while (std::getline(inputfile, line)) {
			if (line.length() > 0) { // ignore blank lines
                std::stringstream ss(line);
                std::string first;
                double second;
                ss >> first >> second;
                fileoutputs[first] = second;
			}
		}
        inputfile.close();
        
        // set appropriate inputs
        for (std::pair<std::string, std::string> otoi : fileentry.second) { // go through the mappings for this file
           int pos = std::find(inputNames.begin(), inputNames.end(), otoi.second) - inputNames.begin(); // get the neural net's input index for the input name
           if (pos < inputNames.size()) {
               inputs[pos] = fileoutputs[otoi.first];
           } else {
               std::cerr << "No input named '" << otoi.second << "' exists." << std::endl;
           }
        }
    }
    
    // Calculate outputs
    std::vector<double> outputs;
    /*******************************************************/
    /**************** INSERT YOUR CODE HERE ****************/
    /*******************************************************/
    for (double d : inputs) outputs.push_back(-d); // simply negate each input
    /*******************************************************/
    
    // Save outputs
    std::ofstream outputsfile(outputFile);
    int i = 0;
    for (double output : outputs) {
        outputsfile << outputNames[i] << " " << output << std::endl;
        i++;
    }
    outputsfile.close();
}

void ChildHost::runCoordinatorCommand() {
    // Note: we are using runCommands instead of runCommand to prevent data overwrite race conditions caused by the coordinator
    pid_t mypid = getpid();
    runCommands(strdup(std::string(TMP_DIR + std::to_string(mypid) + ".command").c_str()));
}

volatile sig_atomic_t usr_interrupt_1 = 0; ///< set when SIGUSR1 arrives
volatile sig_atomic_t usr_interrupt_2 = 0; ///< set when SIGUSR2 arrives
void synch_signal(int sig) {
    if (sig == SIGUSR1) {
        usr_interrupt_1 = 1;
    } else if (sig == SIGUSR2) {
        usr_interrupt_2 = 1;
    }
}


void ChildHost::runAsChild() {
    runAsChildInterruptHandler();
}

void ChildHost::runAsChildInterruptHandler() {
/// @todo maybe this below code block only needs to be called once, maybe put in runAsChild

    // Establish the signal handler
    signal(SIGUSR1, synch_signal);
    signal(SIGUSR2, synch_signal);

    sigset_t mask, oldmask;
    // Set up the mask of signals to temporarily block
    sigemptyset (&mask);
    sigaddset (&mask, SIGUSR1);
    sigaddset (&mask, SIGUSR2);

    // Wait for a signal to arrive
    sigprocmask (SIG_BLOCK, &mask, &oldmask);
    while (!usr_interrupt_1 && !usr_interrupt_2)
        sigsuspend (&oldmask);
    sigprocmask (SIG_UNBLOCK, &mask, NULL);

    // Check which interrupt and continue execution
    if (usr_interrupt_1) {
        usr_interrupt_1 = 0;
        update();
    }
    if (usr_interrupt_2) {
        usr_interrupt_2 = 0;
        runCoordinatorCommand();
    }

    runAsChildInterruptHandler(); // wait for next interrupt
}

void ChildHost::runWithREPL() {
    std::cout << "\033[0;37mChild REPL:\033[0m" << std::endl;
    std::string line;
    std::cout << "\033[0;37m%\033[0m ";
    while (std::getline(std::cin, line)) {
        if (line[0] != '#') { // ignore comments
			if (line.length() > 0) { // ignore blank lines
			    if (line == "quit" || line == "q") {
                    break;
			    } else {
                    if (!runCommand(line)) {
                        std::cerr << "Command \""+line+"\" failed" << std::endl;
                    }
                }
            }
        }
        std::cout << "\033[0;37m%\033[0m ";
    }
}

bool ChildHost::runCommands(char *filepath) {
    bool status = true; // success until a command fails
    std::ifstream infile(filepath);
    std::string line;
    int linenumber = 1;
    while (std::getline(infile, line)) {
        if (line[0] != '#') { // ignore comments
			if (line.length() > 0) { // ignore blank lines
                bool success = runCommand(line);
                if (!success) {
                    status = false;
                    std::cerr << "Command \""+line+"\" at line "+std::to_string(linenumber)+" failed" << std::endl;
                }
            }
        }
        linenumber++;
    }
    if (!status) std::cerr << std::endl;
    return status;
}

bool ChildHost::runCommand(std::string command) {
    if (command == "") return true;
    
    std::string::size_type pos = command.find(' ',0);
    std::string arguments = (pos != command.length()) ? command.substr(pos+1) : "";
    std::string opcode = command.substr(0,pos);
    
    std::string firstarg, secondarg, thirdarg;
    std::istringstream args(arguments);
    args >> firstarg >> secondarg >> thirdarg;
    
    if (opcode == "print") { // print out a string
        std::cout << "OUT: " << arguments << std::endl;
    } else if (opcode == "addinputmapping") {
        addInputMapping(firstarg, secondarg, thirdarg);
    } else if (opcode == "setoutputfile") {
        outputFile = firstarg;
    } else if (opcode == "update") {
        update();
    } else if (opcode == "debug") {
        std::cout << "DEBUG: not implemented in this distribution, not required, no standardized functionality" << std::endl;
    } else {
        std::cerr << "\\/: unknown opcode \"" << opcode << "\"" << std::endl; std::cerr << command;
        return false;
    }
    
    return true;
}

void ChildHost::addInputMapping(std::string outputfilename, std::string outputname, std::string inputname) {
    inputMappings[outputfilename][outputname] = inputname;
}
