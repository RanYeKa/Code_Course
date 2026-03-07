#!/bin/bash
clear
echo "Building the main application..."
gcc -Wall -Wextra -g main.c src/*.c -Iinc -o main_app
