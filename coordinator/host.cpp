/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include "host.h"

Host::Host(char *nconfigpath) {
    started = false;
    hasSentMappings = false;
    targetUpdateInterval = 1.f;
    
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
    std::cout << "\033[0;37mCoordinator REPL:\033[0m" << std::endl;
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
    std::cout << prefix << "Children: " << std::endl;
    if (children.size() == 0) {
        std::cout << prefix << "  none" << std::endl;
    } else {
        for (std::pair<std::string, Child> child : children) {
            if (started && pids.find(child.first) != pids.end())
                std::cout << prefix << "  " << child.first << " (" << pids[child.first] << ")" << std::endl;
            else
                std::cout << prefix << "  " << child.first << std::endl;
        }
    }
    std::cout << prefix << "Target Update Interval: " << std::endl;
    std::cout << prefix << "  " << targetUpdateInterval << " s" << std::endl;
    
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
    std::cout << prefix << "Children: " << std::endl;
    if (children.size() == 0) {
        std::cout << prefix << "  none";
    } else {
        std::cout << prefix << "  " << children.size();
    }
    std::cout << std::endl;
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
    
    pos = arguments.find(' ',0);
    std::string secondandbeyondarguments = (pos != arguments.length()) ? arguments.substr(pos+1) : "";
    
    if (opcode == "print") { // print out a string
        std::cout << "OUT: " << arguments << std::endl;
    } else if (opcode == "summary") { // print out a summary of the network
        printSummary("OUT: ");
    } else if (opcode == "stats") {
        printStats("OUT: ");
    } else if (opcode == "addchild") {
        addChild(firstarg, secondandbeyondarguments); /// @todo potentially perform some type of sanitization on secondandbeyondarguments
    } else if (opcode == "removechild") {
        removeChild(firstarg);
    } else if (opcode == "start") {
        start();
    } else if (opcode == "run") {
        run();
    } else if (opcode == "updateall") {
        updateChildren();
    } else if (opcode == "targetinterval") {
        targetUpdateInterval = std::stof(firstarg);
    } else if (opcode == "runcommand") {
        childRunCommand(firstarg, secondandbeyondarguments);
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

void Host::childRunCommand(std::string name, std::string command) {
    if (!started) {
        std::cerr << "Must run start before run!" << std::endl;
        return;
    }
    
    if (pids.find(name) != pids.end()) { // the child exists
        if (command == "update") { // shortcut for update commands
            kill(pids[name], SIGUSR1);
            return;
        }
        
        // write command to tmp file
        std::ofstream pidfile("/tmp/emergence-neuralnet/" + std::to_string(pids[name]) + ".command");
        pidfile << command;
        pidfile.close();
        
        kill(pids[name], SIGUSR2); // send signal
        kill(pids[name], SIGUSR1); /// @todo this should not be required, but it solves a problem that SIGUSR2 won't arrive until another SIGUSR1 arrives
    } else {
        std::cerr << "No such child, or child process is not running!" << std::endl;
    }
}

void Host::sendMappings() {
    for (std::pair<std::string, std::map<std::string, std::map<std::string, std::string>>> childentry : systemInputMappings) {
        childRunCommand(childentry.first, "setoutputfile /tmp/emergence-neuralnet/" + childentry.first + ".output");
        for (std::pair<std::string, std::map<std::string, std::string>> fileentry : systemInputMappings[childentry.first]) {
            for (std::pair<std::string, std::string> mapping : systemInputMappings[childentry.first][fileentry.first]) {
                childRunCommand(childentry.first, "addinputmapping /tmp/emergence-neuralnet/" + fileentry.first + ".output" + " " + mapping.first + " " + mapping.second);
                /// @todo batch these commands together, eventually
            }
        }
    }
    hasSentMappings = true;
}

void Host::updateChildren() {
    if (!started) {
        std::cerr << "Must run start before run!" << std::endl;
        return;
    }
    
    if (!hasSentMappings) {
        sendMappings();
    }

    updateOscillators();
    timeIndex += targetUpdateInterval;
    
    for (std::pair<std::string, pid_t> child : pids) {
        kill(child.second, SIGUSR1);
    }
}

void Host::updateOscillators() {
    std::ofstream funcfile("/tmp/emergence-neuralnet/oscillators.output");
    funcfile << "sin1 " << sin(timeIndex * 2*M_PI) << std::endl; // sine wave, period = 1 second
    funcfile << "sin8 " << sin(timeIndex * 10 * 2*M_PI) << std::endl; // sine wave, period = 8 seconds
    funcfile << "cos1 " << cos(timeIndex * 2*M_PI) << std::endl; // cosine wave, period = 1 second
    funcfile << "cos8 " << cos(timeIndex * 10 * 2*M_PI) << std::endl; // cosine wave, period = 8 seconds
    funcfile.close();
}

void Host::run() {
    if (!started) {
        std::cerr << "Must run start before run!" << std::endl;
        return;
    }
    
    if (!hasSentMappings) {
        sendMappings();
    }
    
    timeIndex = 0; // reset time index
    
    bool looping = true;
    while (looping) {
        updateOscillators();
        updateChildren();
        sleep(targetUpdateInterval);
        timeIndex += targetUpdateInterval;
    }
}

void Host::start() {
    if (started) {
        std::cout << "OUT: Restarting child processes..." << std::endl;
        killChildren();
    }
    
    started = true;
    
    for (std::pair<std::string, Child> child : children) {
        pid_t pid = fork();
        pids.insert(std::pair<std::string, pid_t>(child.first, pid)); // save pid

        int fd[2]; // file descriptors
        pipe(fd);

        if (pid == 0) { // CHILD PROCESS
            
            // Reassign stdout to fds[1] end of pipe. (i.e. send child output to parent)
            dup2(fd[0], 0);
            // Not going to read in this child process, so we can close this end of the pipe. (i.e. don't send parent input to child)
            close(fd[1]);
            
            // convert to proper c format
            int argc = child.second.argv.size();
            char **argv = new char*[1024];
            for (size_t a=0; a < argc; ++a) argv[a] = const_cast<char*>(child.second.argv[a].c_str());
            argv[argc] = NULL;
            
            int rc = execvp(child.second.invocation.c_str(), argv);
            if (rc == -1) {
                if (errno == 2) {
                    std::cerr << "Child process failed to start. No such file or directory.";
                } else {
                    fprintf(stderr, "Child process failed to start. %d\n", errno);
                }
            }
            perror("command error");
            exit(1);
        } else { // PARENT PROCESS
            std::cout << "OUT: " << "Starting child " << child.first << "..." << std::endl;
        }
    }
}

void Host::killChildren() {
    for (std::pair<std::string, pid_t> pid : pids) {
        kill(pid.second, SIGTERM); // kill the existing child
    }
    pids.clear();
}

void Host::addChild(std::string name, std::string invocation) {
    std::cout << "OUT: " << "Adding new child..." << std::endl;
    std::istringstream iss(invocation);
    std::string first;
    iss >> first;
    std::string token;
    std::vector<std::string> tokens;
    tokens.push_back(first);
    while (iss >> token) {
        tokens.push_back(token);
    }
    Child c(first, tokens);
    children.insert(std::pair<std::string, Child>(name, c));
}

void Host::removeChild(std::string name) {
    children.erase(name);
}

void Host::saveConfiguration() {
    std::ofstream configfile(configpath);
    configfile << "# Children:" << std::endl;
    for (std::pair<std::string, Child> child : children) {
        configfile << child.first << " "; //<< child.second.invocation << " ";
        for (std::string tok : child.second.argv)
            configfile << tok << " ";
        configfile << std::endl;
    }
    
    configfile << std::endl << "# I/O mappings:";
    configfile << std::endl << "# Format: outputchild outputname childname mapped-input" << std::endl;
    for (std::pair<std::string, std::map<std::string, std::map<std::string, std::string>>> childentry : systemInputMappings) {
        for (std::pair<std::string, std::map<std::string, std::string>> fileentry : systemInputMappings[childentry.first]) {
            for (std::pair<std::string, std::string> mapping : systemInputMappings[childentry.first][fileentry.first]) {
                configfile << fileentry.first << " \t" << mapping.first << " \t" << childentry.first << " \t" << mapping.second << std::endl;
            }
        }
    }
    
    configfile << std::endl << "# Parameters:" << std::endl;
    configfile << "targetUpdateInterval " << targetUpdateInterval << std::endl;
    configfile.close();
    
    std::cout << "OUT: " << "System configuration succesfully saved" << std::endl;
}


void Host::readConfigFile() {
    std::ifstream filestream(configpath);
    std::string line;
    int section = 0;
    while (std::getline(filestream, line)) {
        if (line[0] != '#') { // ignore comments
			if (line.length() > 0) { // check blank lines
                std::istringstream iss(line);
        
                if (section == 0) { // CHILDREN
                    std::string::size_type pos = line.find(' ',0);
                    std::string invocation = (pos != line.length()) ? line.substr(pos+1) : "";
                    std::string name = line.substr(0,pos);
                    
                    std::istringstream iss(invocation);
                    std::string first;
                    iss >> first;
                    std::string token;
                    std::vector<std::string> tokens;
                    tokens.push_back(first);
                    while (iss >> token) {
                        tokens.push_back(token);
                    }
                    Child c(first, tokens);
                    children.insert(std::pair<std::string, Child>(name, c));
                } else if (section == 1) { // I/O MAPPINGS
                    std::string outputfile, outputname, childname, mappedinput;
                    iss >> outputfile >> outputname >> childname >> mappedinput;
                    systemInputMappings[childname][outputfile][outputname] = mappedinput;
                } else if (section == 2) { // PARAMETERS
                    std::string parameter;
                    iss >> parameter;
                    if (parameter == "targetUpdateInterval") {
                        iss >> targetUpdateInterval;
                    }
                }
            } else {
                section++;
            }
        }
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
    }
    // if (linenum < 3) {
        // std::cerr << "ERROR: Malformed structure file!";
    // }
}