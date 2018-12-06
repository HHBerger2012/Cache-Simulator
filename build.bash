#!/bin/bash
export compile="gcc -fPIC -std=c99 -Wall  -c -g"

$compile -o objs/command_line.o command_line.c

gcc -shared -fPIC -o libname.so objs/command_line.o

gcc -L/home/aplaza/hwk_8/ -std=c99 -Wall -g \
    -o shell_main shell_main.c\
    -lname


