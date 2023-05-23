#!/bin/bash
#rm data.exe
mpic++ --std=c++17 data.cpp -o data.exe
mpiexec -np 2 data.exe
#clang++ --std=c++17 data.cpp -o data.exe
