#!/bin/bash

# OPtional input parameter is DEBUG or RELEASE
BUILD_TYPE=$1

mkdir build
cd build

cmake -S ../src -B . -DCMAKE_BUILD_TYPE=$BUILD_TYPE
make -j
