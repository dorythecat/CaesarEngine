#!/bin/bash

[ ! -d "build" ] && mkdir build
cd build
cmake ..
make -j$(nproc --ignore=2)
cd ..
