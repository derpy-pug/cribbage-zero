#!/bin/bash

cd build

make test || exit 1
