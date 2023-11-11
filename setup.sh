#!/bin/bash

mkdir build
cd build

cmake -S ../src -B . -DCMAKE_BUILD_TYPE=Release
make -j
