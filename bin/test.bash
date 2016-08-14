#!/bin/bash

cd ../
./buildrun.bash &> /dev/null
./a.out NOP
./a.out write /tmp/foo < ./src/main.c > example.journal.log.txt
./a.out read /tmp/foo < example.journal.log.txt | diff - ./src/main.c
