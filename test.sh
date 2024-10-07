#!/bin/bash

cd build

make test
ret=$?

cd ..

exit $ret
