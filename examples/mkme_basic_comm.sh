#!/bin/bash
rm basic_comm.exe
mpic++ --std=c++17 -g basic_comm.cpp -o basic_comm.exe -Wall -Wextra
mpiexec -np 3 basic_comm.exe
