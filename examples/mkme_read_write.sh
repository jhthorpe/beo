#!/bin/bash
rm read_write.exe
mpic++ --std=c++17 -g read_write.cpp -o read_write.exe -Wall -Wextra
mpiexec -np 3 read_write.exe
