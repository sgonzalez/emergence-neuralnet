#!/bin/bash

cd child_feedforward
Echo "Compiling feedforward..."
make
cd ..

cd coordinator
Echo "Compiling coordinator..."
make
cd ..