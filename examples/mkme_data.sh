#!/bin/bash
mpic++ --std=c++17 data.cpp -o data.exe
mpiexec -np 2 data.exe
