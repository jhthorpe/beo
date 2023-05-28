#!/bin/bash
rm basic_comm.exe
mpic++ --std=c++17 basic_comm.cpp -o basic_comm.exe
mpiexec -np 3 basic_comm.exe
