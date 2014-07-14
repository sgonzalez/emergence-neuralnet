# Emergence Neural Network Project
This project introduces an extremely flexible, scalable, and easy-to-use tool for the construction, learning, testing, and real-world usage of neural networks. Each neural network system can be composed of one single neural network process, or several neural network and input/output driver processes that are managed by one coordinating command process, communicating through the use of tmp files. The neural networks each cater their own set of commands which are used to manipulate and modify the neural network. These commands are accessible through a user friendly REPL (Read Evaluate Print Loop) or specified command files. Additionally, each neural network is capable of learning through the use of genetic algorithms faculties that have been included. (Backpropagation supervised learning may be included at a future date)

### Compiling
To compile, just run ```build.sh``` from the root directory to compile everything, or ```make``` from each child program directory individually. There are no weird dependencies. However, I have not attempted to compile this on Windows and can't guarantee that everything will work "out of the box".

### Licensing
The code for Emergence is hereby released under the GNU General Public License (GPL) v2. Specific terms are available in the ```LICENSE``` file. Essentially, Emergence cannot be used in commercial (i.e. paid) software, uses of Emergence must retain attribution, and modifications to Emergence must be re-released into open-source under the same license. If you would like to use emergence in a commercial software distribution, feel free to contact me at slgonzalez (at) me (dot) com.

### Example Application
An example application has been included and fully configured for you to try out.

---------------------------------------

## Coordinator
The coordinator (also called the command process) manages each child process and coordinates input/output funneling and network updates. Interprocess data communication is achieved through the use of ```/tmp``` files (most systems mount ```/tmp``` to RAM instead of disk). These files are managed by the coordinator. The coordinator uses a user specified file to store information, much like the feedforward child process has structure and weight files. The coordinator is capable of generating oscillatory inputs and propagating global inputs.

### XPC File Format
All XPC files are stored in ```/tmp/emergence-neuralnet```. This directory gets deleted when the coordinator terminates; to prevent this behavior from taking place, pass the ```--tmpkeep``` to the coordinator. Each data communication file uses the following format to communicate data:

    outputname: value
    anotheroutput: value
    computeddirection: 0.23
    etc. etc. etc.

*Note:* each output name is the output name from the process that inputs (i.e. writes) data into the file

### Notes
THIS SHOULD CHANGE TO BE BETTER!!!! WHAT IF OUTPUTS ARE IN DIFFERENT ORDER OR HAVE DIFFERENT NAMES???? NEED SOME TYPE OF USER SPECIFICATION MECHANISM
* When using ```setinputfile filepath```, inputs are filled from left to right (i.e. from 0 to n) until, either there are no more inputs to fill, or all inputs in the input file have been read. This means that all the other inputs have to be set using the ```input name value``` command.
* Partially due to the above semantic rule, special inputs (i.e. oscillatory inputs and global inputs) have to be an "unfilled" input.
* The coordinator configuration file is in an easy to use, human readable format.

### Coordinator Commands
* ```quit``` or ```q```: quits the REPL
* ```print STRING```: prints out a string (the remainder of the line)
* ```run```: runs the system with the current configuration, must run ```start``` first. Does not return. Use SIGINT (i.e. <kbd>ctrl</kbd>-<kbd>c</kbd>) to stop.
* ```targetinterval seconds```: sets the system's target update interval to ```seconds```
* ```updateall```: updates all the children, useful for testing
* ```start```: (re)starts execution of the entire network's processes
* ```summary```: prints out a summary of the current structure of the network
* ```stats```: prints out various network statistics
* ```addchild name INVOCATION```: adds a new child to be managed by the cooordinator, referenced by ```name``` and run by calling ```INVOCATION``` (note:  be careful when using this command from an external program, ```INVOCATION``` is *not* sanitized to grant you the ability to write your own children). Make sure that the child is set to run without a REPL. As a convention, either use absolute paths for files, or use paths relative to the coordinator. Make sure that you are invoking the program as a child.
* ```removechild name```: removes the child with ```name```
* ```save```: saves the system's configuration to the persistence file
* N```runcommand name COMMAND```

### Typical Command Flow
Here is a sample sequence of commands that can be used with the coordinator, either in the REPL or a command file. Note how there are no commands after ```run``` since control will never reach them.

    newchild neural1 ../child_feedforward/feedforward -c ../examples/test.structure ../examples/test.weights 
    newchild neural2 ../child_feedforward/feedforward -c ../examples/blah.structure ../examples/blah.weights 
    targetinterval 0.1
    save
    start
    run

### Child Process Commands
All child processes have to support these commands to be supported by the coordinator:
* ```quit``` or ```q```: quits the REPL
* N```setinputfile filepath```: sets the file from which inputs are taken (readonly)
* N```addinputmapping ouputname inputname``` maps an output from an XPC file to an input, unmapped / unfilled inputs default to zero
* N```setoutputfile filepath```: sets the file where outputs are written (writeonly)
* N```input name value```: sets a specific input
* N```update```: update outputs


## Child Feedforward
A standard multilayer perceptron neural network. You can quickly try out this component by running:

    ./feedforward -C ../examples/test.commands ../examples/test.structure ../examples/test.weights

from within the ```child_feedforward``` directory.

### Notes
* All layers are fully connected (i.e. in every neuron in a layer has a connecting dendrite to each neuron in the upstream layer). Every pair of adjacent layers forms a fully-connected, bipartite graph.


### Commands
* ```print STRING```: prints out a string (the remainder of the line)
* ```summary```: prints out a summary of the current structure of the neural network
* ```stats```: prints out the number of neurons and synapses in the network
* ```reset```: deletes all neurons and inputs, restores to starting neural network
* ```save```: saves the network structure and weights
* ```randomize```: rerandomizes all the weights
* ```zeroweights```: sets all the weights to zero
* ```layeradd index numneurons```: adds a hidden layer to the network with ```numneurons``` neurons at ```index```
* ```layerremove index```: removes the hidden layer at ```index```
* ```inputadd name```: adds a new input to the neural network named ```name```, names with weird characters may result in undefined behavior (stick with alphanumeric names for now)
* ```inputremove name```: removes the input named ```name``` from the neural network
* N```inputbulkadd name quantity```: 
* N```inputbulkremove name```: 
* ```outputadd name```: adds a new output neuron
* ```outputremove name```: removes an output neuron
* ```neuronadd index numneurons```: adds ```numneurons``` neurons to the layer at ```index```
* ```neuronremove index numneurons```: removes ```numneurons``` neurons from the layer at ```index```



### TODO
* Create new structure and weights files if they don't exist
* DONE! Think about and document command semantics (e.g. insertion strategies, what to do with weights, etc.)


## Image Input Driver
An image input driver suitable for image recognition and other tasks.


## Arduino Input/Output Driver
This driver is suitable for communicating with an Arduino through a USB serial port link.