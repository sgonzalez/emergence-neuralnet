/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include "neuralhost.h"

NeuralHost::NeuralHost(char *nstructurepath, char *nweightspath) {
    srand(time(NULL)); // seed the prng
    
    structurepath = nstructurepath;
    weightspath = nweightspath;
    
    // read neuralnet if it already exists
    if (access(structurepath, R_OK) != -1) { // make sure the structure file is accessible
        readStructureFile();
        if (access(weightspath, R_OK) != -1) { // make sure the weights file is accessible
            readWeightsFile();
        }
    }
    
    // make sure the provided files are writable
    if (!( access(structurepath, W_OK) != -1 && access(weightspath, W_OK) != -1 )) {
        std::cerr << "structure and weights files do not have write permissions or do not exist" << std::endl;
    }
}

void NeuralHost::update() {
    std::cout << "YO DAWG!!" << std::endl;
}

/* When a SIGUSR1 signal arrives, set this variable. */
volatile sig_atomic_t usr_interrupt_1 = 0;
volatile sig_atomic_t usr_interrupt_2 = 0;
void synch_signal(int sig) { 
    if (sig == SIGUSR1)
        usr_interrupt_1 = 1;
    else if (sig == SIGUSR2)
        usr_interrupt_2 = 1;
}


void NeuralHost::runAsChild() {
    runAsChildInterruptHandler();
}

void NeuralHost::runAsChildInterruptHandler() {
    struct sigaction usr_action;
    sigset_t block_mask;
    pid_t child_id;

    /* Establish the signal handler. */
    sigfillset (&block_mask);
    usr_action.sa_handler = synch_signal;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    sigset_t mask, oldmask;
    /* Set up the mask of signals to temporarily block. */
    sigemptyset (&mask);
    sigaddset (&mask, SIGUSR1);

    /* Wait for a signal to arrive. */
    sigprocmask (SIG_BLOCK, &mask, &oldmask);
    while (!usr_interrupt_1)
        sigsuspend (&oldmask);
    sigprocmask (SIG_UNBLOCK, &mask, NULL);

    /* Now continue execution. */
    usr_interrupt_1 = 0;
    update();

    runAsChildInterruptHandler();
}

void NeuralHost::runWithREPL() {
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

bool NeuralHost::runCommands(char *filepath) {
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

void NeuralHost::printSummary(std::string prefix) {
    std::cout << prefix << "----------------" << std::endl;
    std::cout << prefix << "Outputs:" << std::endl;
    if (neuralnet.getOutputs().size() > 0) {
        std::cout << prefix << "  " << neuralnet.getLayers().size() << ": (x" << neuralnet.getOutputs().size() << ") ";
        for (std::string output : neuralnet.getOutputs()) std::cout << output << " ";
    } else {
        std::cout << prefix << "  none";
    }
    std::cout << std::endl;    
    std::cout << prefix << "Hidden Layers:" << std::endl;
    if (neuralnet.getLayers().size() > 1) {
        for (int i = neuralnet.getLayers().size()-2; i > -1; i--)
            std::cout << prefix << "  " << i+1 << ": (x" << neuralnet.getLayers()[i].neurons.size() << ") " << std::endl;
    } else {
        std::cout << prefix << "  none" << std::endl;
    }
    std::cout << prefix << "Inputs:" << std::endl;
    if (neuralnet.getInputs().size() > 0) {
        std::cout << prefix << "  " << 0 << ": (x" << neuralnet.getInputs().size() << ") ";
        for (std::string input : neuralnet.getInputs()) std::cout << input << " ";
    } else {
        std::cout << prefix << "  none";
    }
    std::cout << std::endl;
    std::cout << prefix << "----------------" << std::endl;
}

void NeuralHost::printStats(std::string prefix) {
    std::cout << prefix << "----------------" << std::endl;
    std::cout << prefix << "Neurons: " << std::endl;
    if (neuralnet.getLayers().size() > 0 || neuralnet.getOutputs().size() > 0) {
        int sum = 0;
        for (int i = 0; i < neuralnet.getLayers().size(); i++)
            sum += neuralnet.getLayers()[i].numNeurons;
        std::cout << prefix << "  " << sum;
    } else {
        std::cout << prefix << "  none";
    }
    std::cout << std::endl;    
    std::cout << prefix << "Synapses:" << std::endl;
    if (neuralnet.getLayers().size() > 1 || neuralnet.getInputs().size() > 0) {
        int sum = 0;
        for (int i = 0; i < neuralnet.getLayers().size(); i++)
            sum += neuralnet.getLayers()[i].numInputsPerNeuron * neuralnet.getLayers()[i].numNeurons;
        std::cout << prefix << "  " << sum << std::endl;
    } else {
        std::cout << prefix << "  none" << std::endl;
    }
    std::cout << prefix << "----------------" << std::endl;
}

bool NeuralHost::runCommand(std::string command) {
    std::string::size_type pos = command.find(' ',0);
    std::string arguments = (pos != command.length()) ? command.substr(pos+1) : "";
    std::string opcode = command.substr(0,pos);
    
    std::string firstarg, secondarg;
    std::istringstream args(arguments);
    args >> firstarg >> secondarg;
    
    if (opcode == "print") { // print out a string
        std::cout << "OUT: " << arguments << std::endl;
    } else if (opcode == "summary") { // print out a summary of the neural network
        printSummary("OUT: ");
    } else if (opcode == "stats") {
        printStats("OUT: ");
    } else if (opcode == "save") { // persists the neural network to the output files
        saveNetwork();
    } else if (opcode == "reset") { // resets the neural network to a "fresh" configuration
        neuralnet = NeuralNet();
    } else if (opcode == "randomize") { // randomizes all the weights in the neural network
        neuralnet.randomizeWeights();
    } else if (opcode == "zeroweights") { // zeroes all the weights in the neural network
        neuralnet.zeroWeights();
    } else if (opcode == "inputadd") { // add an input to the neural network
        neuralnet.addInput(firstarg);
    } else if (opcode == "outputadd") { // add an output neuron to the neural network
        neuralnet.addOutput(firstarg);
    } else if (opcode == "inputremove") { // remove an input from the neural network
        neuralnet.removeInput(firstarg);
    } else if (opcode == "outputremove") { // remove an output neuron from the neural network
        neuralnet.removeOutput(firstarg);
    } else if (opcode == "neuronadd") { // add a specified number of neurons to a hidden layer
        neuralnet.addNeurons(std::stoi(firstarg), std::stoi(secondarg));
    } else if (opcode == "neuronremove") { // remove a specified number of neurons from a hidden layer
        neuralnet.removeNeurons(std::stoi(firstarg), std::stoi(secondarg));
    } else if (opcode == "layeradd") {
        neuralnet.addLayer(std::stoi(firstarg), std::stoi(secondarg));
    } else if (opcode == "layerremove") {
        neuralnet.removeLayer(std::stoi(firstarg));
    } else if (opcode == "timepropagation") {
        timePropagation();
    } else if (opcode == "debug") {
        std::cout << "DEBUG: not implemented in this distribution, not required, no standardized functionality" << std::endl;
        // for (NeuronLayer layer : neuralnet.getLayers())
            // std::cout << layer.numNeurons << " " << layer.numInputsPerNeuron << std::endl;
    } else {
        std::cerr << "\\/: unknown opcode \"" << opcode << "\"" << std::endl;
        return false;
    }
    
    return true;
}

void NeuralHost::timePropagation() {
    static const int iterations = 100;
    int numInputs = neuralnet.getInputs().size();
    clock_t begin = clock();
    for (int i = 0; i < iterations; i++) {
        std::vector<double> inputs;
        for (int j = 0; j < numInputs; j++) {
            inputs.push_back(randomClamped());
        }
        neuralnet.propagate(inputs);
    }
    clock_t end = clock();
    double elapsedSeconds = (double(end - begin) / CLOCKS_PER_SEC) / iterations;
    std::cout << "OUT: " << "Neural network propagation time: ";
    printf("%.4lf seconds / %.4lf milliseconds", elapsedSeconds, elapsedSeconds*1000);
    std::cout << std::endl;
}

void NeuralHost::saveNetwork() {
    // save structure
    std::ofstream structurefile(structurepath);
    for (std::string input : neuralnet.getInputs())
        structurefile << input << " ";
    structurefile << std::endl;
    for (std::string output : neuralnet.getOutputs())
        structurefile << output << " ";
    structurefile << std::endl;
    for (NeuronLayer layer : neuralnet.getLayers())
        structurefile << layer.numNeurons << " " << layer.numInputsPerNeuron << std::endl;
    structurefile.close();
    
    // save weights
    std::ofstream weightsfile(weightspath);
    for (double w : neuralnet.getWeights()) weightsfile << w << " ";
    weightsfile.close();
    
    std::cout << "OUT: " << "Neural network succesfully saved" << std::endl;
}


void NeuralHost::readStructureFile() {
    std::ifstream filestream(structurepath);
    std::string line;
    int linenum = 0;
    while (std::getline(filestream, line)) {
        std::istringstream iss(line);
        if (linenum == 0) { // inputs
            std::string name;
            while (iss >> name) {
                neuralnet.addInput(name);
            }
        } else if (linenum == 1) { // outputs
            std::string name;
            while (iss >> name) {
                neuralnet.addOutput(name);
            }
        } else { // layers
            int numNeurons, numInputsPerNeuron;
            if (!(iss >> numNeurons >> numInputsPerNeuron)) {
                std::cerr << "ERROR: Malformed structure file!";
            } else {
                neuralnet.addLayerBeforeOutputLayer(numNeurons, numInputsPerNeuron);
            }
        }
        linenum++;
    }
    if (linenum < 3) {
        std::cerr << "ERROR: Malformed structure file!";
    }
}

void NeuralHost::readWeightsFile() {
    std::ifstream filestream(weightspath);
    std::vector<double> loadedWeights;
    
    double weight;
    while (filestream >> weight) loadedWeights.push_back(weight);
    
    int expectedNum = neuralnet.getNumberOfWeights();
    if (expectedNum != loadedWeights.size()) {
        std::cerr << "ERROR: Could not load weights file! Expected " << expectedNum << ", received " << loadedWeights.size() << " weights." << std::endl;
    } else {
        neuralnet.setWeights(loadedWeights);
    }
}