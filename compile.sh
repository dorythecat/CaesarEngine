#!/bin/bash

[! -d "build" ] && mkdir build
cd build
cmake ../src
make
cd ..
