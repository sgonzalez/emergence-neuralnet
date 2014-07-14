/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include "host.h"

Host::Host(char *nconfigpath) {
    
    configpath = nconfigpath;
    
    // read configuration if it already exists
    if (access(configpath, R_OK) != -1) { // make sure the config file is accessible
        readConfigFile();
    }
    
    // make sure the provided file is writable
    if (access(configpath, W_OK) == -1) {
        std::cerr << "configuration file does not have write permissions or does not exist" << std::endl;
    }
}

void Host::runWithREPL() {
    std::cout << "Coordinator REPL:" << std::endl;
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

bool Host::runCommands(char *filepath) {
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

void Host::printSummary(std::string prefix) {
    std::cout << prefix << "----------------" << std::endl;
    // std::cout << prefix << "Outputs:" << std::endl;
    //     if (neuralnet.getOutputs().size() > 0) {
    //         std::cout << prefix << "  " << neuralnet.getLayers().size() << ": (x" << neuralnet.getOutputs().size() << ") ";
    //         for (std::string output : neuralnet.getOutputs()) std::cout << output << " ";
    //     } else {
    //         std::cout << prefix << "  none";
    //     }
    //     std::cout << std::endl;    
    //     std::cout << prefix << "Hidden Layers:" << std::endl;
    //     if (neuralnet.getLayers().size() > 1) {
    //         for (int i = neuralnet.getLayers().size()-2; i > -1; i--)
    //             std::cout << prefix << "  " << i+1 << ": (x" << neuralnet.getLayers()[i].neurons.size() << ") " << std::endl;
    //     } else {
    //         std::cout << prefix << "  none" << std::endl;
    //     }
    //     std::cout << prefix << "Inputs:" << std::endl;
    //     if (neuralnet.getInputs().size() > 0) {
    //         std::cout << prefix << "  " << 0 << ": (x" << neuralnet.getInputs().size() << ") ";
    //         for (std::string input : neuralnet.getInputs()) std::cout << input << " ";
    //     } else {
    //         std::cout << prefix << "  none";
    //     }
    //     std::cout << std::endl;
    std::cout << prefix << "----------------" << std::endl;
}

void Host::printStats(std::string prefix) {
    std::cout << prefix << "----------------" << std::endl;
    // std::cout << prefix << "Neurons: " << std::endl;
    //     if (neuralnet.getLayers().size() > 0 || neuralnet.getOutputs().size() > 0) {
    //         int sum = 0;
    //         for (int i = 0; i < neuralnet.getLayers().size(); i++)
    //             sum += neuralnet.getLayers()[i].numNeurons;
    //         std::cout << prefix << "  " << sum;
    //     } else {
    //         std::cout << prefix << "  none";
    //     }
    //     std::cout << std::endl;    
    //     std::cout << prefix << "Synapses:" << std::endl;
    //     if (neuralnet.getLayers().size() > 1 || neuralnet.getInputs().size() > 0) {
    //         int sum = 0;
    //         for (int i = 0; i < neuralnet.getLayers().size(); i++)
    //             sum += neuralnet.getLayers()[i].numInputsPerNeuron * neuralnet.getLayers()[i].numNeurons;
    //         std::cout << prefix << "  " << sum << std::endl;
    //     } else {
    //         std::cout << prefix << "  none" << std::endl;
    //     }
    std::cout << prefix << "----------------" << std::endl;
}

bool Host::runCommand(std::string command) {
    std::string::size_type pos = command.find(' ',0);
    std::string arguments = (pos != command.length()) ? command.substr(pos+1) : "";
    std::string opcode = command.substr(0,pos);
    
    std::string firstarg, secondarg;
    std::istringstream args(arguments);
    args >> firstarg >> secondarg;
    
    if (opcode == "print") { // print out a string
        std::cout << "OUT: " << arguments << std::endl;
    } else if (opcode == "summary") { // print out a summary of the network
        printSummary("OUT: ");
    } else if (opcode == "stats") {
        printStats("OUT: ");
    } else if (opcode == "save") { // persists the configuration to the output file
        saveConfiguration();
    } else if (opcode == "debug") {
        std::cout << "DEBUG: not implemented in this distribution, not required, no standardized functionality" << std::endl;
    } else {
        std::cerr << "\\/: unknown opcode \"" << opcode << "\"" << std::endl;
        return false;
    }
    
    return true;
}


void Host::saveConfiguration() {
    std::ofstream configfile(configpath);
    // for (double w : neuralnet.getWeights()) weightsfile << w << " ";
    configfile.close();
    
    std::cout << "OUT: " << "System configuration succesfully saved" << std::endl;
}


void Host::readConfigFile() {
    std::ifstream filestream(configpath);
    std::string line;
    int linenum = 0;
    while (std::getline(filestream, line)) {
        std::istringstream iss(line);
        // if (linenum == 0) { // inputs
        //             std::string name;
        //             while (iss >> name) {
        //                 neuralnet.addInput(name);
        //             }
        //         } else if (linenum == 1) { // outputs
        //             std::string name;
        //             while (iss >> name) {
        //                 neuralnet.addOutput(name);
        //             }
        //         } else { // layers
        //             int numNeurons, numInputsPerNeuron;
        //             if (!(iss >> numNeurons >> numInputsPerNeuron)) {
        //                 std::cerr << "ERROR: Malformed structure file!";
        //             } else {
        //                 neuralnet.addLayerBeforeOutputLayer(numNeurons, numInputsPerNeuron);
        //             }
        //         }
        linenum++;
    }
    // if (linenum < 3) {
        // std::cerr << "ERROR: Malformed structure file!";
    // }
}