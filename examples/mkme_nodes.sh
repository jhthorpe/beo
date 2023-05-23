#!/bin/bash
rm nodes.exe
mpic++ --std=c++17 nodes.cpp -o nodes.exe
mpiexec -np 2 nodes.exe
