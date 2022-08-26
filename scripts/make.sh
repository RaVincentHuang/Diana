#!/bin/bash
cd ../build || exit
cmake .. -DCMAKE_CXX_COMPILER=clang++
make
cd ../sh_scripts || exit
