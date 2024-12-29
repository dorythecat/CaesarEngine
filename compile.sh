#!/bin/bash

[ ! -d "build" ] && mkdir build
cd build
cmake cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc --ignore=2)
cd ..
