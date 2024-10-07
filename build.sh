#!/bin/bash

# OPtional input parameter is DEBUG or RELEASE
BUILD_TYPE=$1

if [ ! -d "build" ]; then
    mkdir build
fi
cd build

cmake -S ../src -B . -DCMAKE_BUILD_TYPE=$BUILD_TYPE
make -j

cd ..

ln -sf build/app/app ./cribbage
