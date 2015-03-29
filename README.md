# Emergence Neural Network Project
This project introduces an extremely flexible, scalable, and easy-to-use tool for the construction, learning, testing, and real-world usage of neural networks and real-time control systems. Each neural network system can be composed of one single neural network process, or several neural network and input/output driver processes that are managed by one coordinating command process, communicating through the use of tmp files. The neural networks each cater their own set of commands which are used to manipulate and modify the neural network. These commands are accessible through a user friendly REPL (Read Evaluate Print Loop) or specified command files. Additionally, each neural network is capable of learning through the use of genetic algorithms faculties that have been included. Genetic algorithms have been selected over backpropagation because backpropagation can take a long time to run for networks with many hidden layers. (Backpropagation supervised learning may be included at a future date)

### Compiling
To compile, just run ```build.sh``` from the root directory to compile everything, or ```make``` from each child program directory individually. There are no weird dependencies. However, I have not attempted to compile this on Windows and can't guarantee that everything will work "out of the box". Everything should run fine on any *nix system (e.g. Linux, Mac, BSD).

### Licensing
The code for Emergence is hereby released under the GNU General Public License (GPL) v2. Specific terms are available in the ```LICENSE``` file. Essentially, Emergence cannot be used in commercial (i.e. paid) software, uses of Emergence must retain attribution, and modifications to Emergence must be re-released into open-source under the same license. If you would like to use emergence in a commercial software distribution, feel free to contact me at slgonzalez (at) me (dot) com.

### Example Application
An example application has been included and fully configured for you to try out. To get started (assuming you have compiled everything), go to the repository's root directory and type the following commands:

    cd coordinator
    coordinator ../examples/test.coordinator
    # The following should be typed within the coordinator REPL
    start
    summary
    updateall


---------------------------------------

## Coordinator
The coordinator process manages each child process and coordinates input/output funneling and network updates. Interprocess communication (XPC) is achieved through the use of ```SIGUSR1```/```SIGUSR2``` kill signals and ```/tmp``` files (most systems mount ```/tmp``` to RAM instead of disk). These files are managed by the coordinator. The coordinator uses a user specified file to store configuration information, much like the feedforward child process has structure and weight files. Currently, the user has to manually modify this configuration file to specify input/output mappings (it is very quick and easy, don't worry). The configuration file is extremely sensitive to having correct empty lines in the right places.

The coordinator is capable of generating oscillatory inputs and propagating global inputs. The coordinator's set of real-time oscillating functions can be used as inputs in the ```oscillators``` output file. This means that you should not have any children named ```oscillators```. Currently ```oscillators``` provides sine and cosine functions. Global inputs are specified in the configuration file and are great for use as placeholders and constants across the system (hence, global). They are stored in the ```globalinputs``` output file. This means that you should not have any children named ```globalinputs```.

### XPC Files
All XPC files are stored in ```/tmp/emergence-neuralnet```. This directory gets deleted when the coordinator terminates; to prevent this behavior from taking place, pass the ```--tmpkeep``` option to the coordinator. Different types of files reside in this directory:

* ```.command``` files: these files contain one or more child commands and are used by the parent to send commands, the number before the file extension is the child's PID
* ```.output``` files: these are the files that contain the actual output data produced by each child (and the coordinator's oscillators and global inputs)

Each output file uses the following format to communicate data:

    outputname value
    anotheroutput value
    computeddirection 0.23
    etc. etc. etc.

*Note:* each output name is the output name from the process that inputs (i.e. writes) data into the file

### Notes
* The coordinator configuration file is in an easy to use, human readable format. Feel free to modify it manually.

### Coordinator Commands
* ```quit``` or ```q```: quits the REPL
* ```print STRING```: prints out a string (the remainder of the line)
* ```run```: runs the system with the current configuration, must run ```start``` first. Does not return. Use SIGTERM or SIGINT (i.e. <kbd>ctrl</kbd>+<kbd>c</kbd>) to stop.
* ```targetinterval seconds```: sets the system's target update interval to a real number of ```seconds```
* ```updateall```: updates every child's outputs based on its inputs, also steps oscillators forward, useful for testing
* ```start```: (re)starts execution of the entire network's processes
* ```summary```: prints out a summary of the current structure of the network
* ```stats```: prints out various network statistics
* ```addchild name INVOCATION```: adds a new child to be managed by the cooordinator, referenced by ```name``` and run by calling ```INVOCATION``` (note:  be careful when using this command from an external program, ```INVOCATION``` is *not* sanitized to grant you the ability to write your own children). Make sure that the child is set to run without a REPL. As a convention, either use absolute paths for files, or use paths relative to the coordinator. Make sure that you are invoking the program as a child.
* ```removechild name```: removes the child with ```name```
* ```save```: saves the system's configuration to the persistence file
* ```runcommand name COMMAND```: run the specified command on the child referenced by ```name```

### Typical Command Flow
Here is a sample sequence of commands that can be used with the coordinator, either in the REPL or a command file. Note how there are no commands after ```run``` since control will never reach them.

    newchild neural1 ../child_feedforward/feedforward -c ../examples/test.structure ../examples/test.weights 
    newchild neural2 ../child_feedforward/feedforward -c ../examples/blah.structure ../examples/blah.weights 
    targetinterval 0.1
    save
    start
    run

### Child Process Commands
All child processes have to support a set of commands to allow manageability by the coordinator. Note that all training is handled by the children themselves, not the coordinator (a global supervised learning type thing might be added in the future).
* ```quit``` or ```q```: quits the REPL
* ```addinputmapping outputfile ouputname inputname``` maps an output from an XPC file to an input, unmapped / unfilled inputs default to zero
* ```setoutputfile filepath```: sets the file where outputs are written (writeonly)
* ```update```: update outputs


## Feedforward Neural Network (Child)
A standard multilayer perceptron neural network. You can quickly try out this component by running:

    ./feedforward -C ../examples/test.commands ../examples/test.structure ../examples/test.weights

from within the ```child_feedforward``` directory.

### Notes
* All layers are fully connected (i.e. in every neuron in a layer has a connecting dendrite to each neuron in the upstream layer). Every pair of adjacent layers forms a fully-connected, bipartite graph. In the future, synapse removal algorithms, such as optimal brain damage, may be implemented.


### Commands
* ```print STRING```: prints out a string (the remainder of the line)
* ```summary```: prints out a summary of the current structure of the neural network
* ```stats```: prints out the number of neurons and synapses in the network
* ```reset```: deletes all neurons and inputs, restores to starting neural network
* ```save```: saves the network structure and weights
* ```randomize```: rerandomizes all the weights
* ```zeroweights```: sets all the weights to zero
* NOT IMPLEMENTED YET```learn``` or ```train```
* ```layeradd index numneurons```: adds a hidden layer to the network with ```numneurons``` neurons at ```index```
* ```layerremove index```: removes the hidden layer at ```index```
* ```inputadd name```: adds a new input to the neural network named ```name```, names with weird characters may result in undefined behavior (stick with alphanumeric names for now)
* ```inputremove name```: removes the input named ```name``` from the neural network
* NOT IMPLEMENTED YET```inputbulkadd name quantity```: 
* NOT IMPLEMENTED YET```inputbulkremove name```: 
* ```outputadd name```: adds a new output neuron
* ```outputremove name```: removes an output neuron
* ```neuronadd index numneurons```: adds ```numneurons``` neurons to the layer at ```index```
* ```neuronremove index numneurons```: removes ```numneurons``` neurons from the layer at ```index```
* ```timepropagation```: profiles the neural network's propagation time (i.e. how long it takes for outputs to change based on the inputs). Actual propagation is run many times with random inputs to ensure a good number.

#### Learning Commands
* ```train trainingfile popsize generations```: trains the neural network with genetic algorithms, using the training data file ```trainingfile```, a population of size ```popsize```, for ```generations``` "generations". The fitness is calculated as an average deviation from the expected values for each sample in the training data file.
* NOT IMPLEMENTED YET ```train trainingfile popsize generations fitness```: similar to above, uses custom fitness function that is loaded at runtime using ```dlopen()```.
* NOT IMPLEMENTED YET```setgenetic```: s
* NOT IMPLEMENTED YET```epoch```: run the learning for another "generation"
* NOT IMPLEMENTED YET```setgenetic```: s
* NOT IMPLEMENTED YET```setgenetic```: s


### TODO
* Create new structure and weights files if they don't exist
* Make the tmp file directory a constant
* Have a "history" in the REPL like a shell does, so you can up-arrow to reuse the previously used command
* Potentially move shared child code into a shared code directory

## Generic (Child)
This is as barebones as a child can get. It implements the basic child functionality (i.e. child process commands, XPC, kill signals, etc.) and performs concrete operations (written within the code, just a negation for now) on the input data to produce outputs in just over 200 lines of code! For actual use, this should be used as a template/reference to create your own child types. Reference ```feedforward``` for more advanced functionality (including configuration persistence).

This base implementation currently just negates the three inputs x, y, z and outputs xn, yn, zn.

## Image Input Driver (Child)
An image input driver suitable for image recognition and other tasks.


## Arduino Input/Output Driver (Child)
This driver is suitable for communicating with an Arduino through a USB serial port link.

