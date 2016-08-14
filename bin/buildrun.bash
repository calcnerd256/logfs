#!/bin/bash

cd ../
gcc -Wall \
 ./src/main-NOP.c \
 ./src/main-write.c \
 ./src/main-read.c \
 ./src/main.c
valgrind ./a.out NOP
