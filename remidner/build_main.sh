#!/bin/bash
clear
echo "Building the main application..."
gcc -Wall -Wextra -g main.c src/* -Iinc -o main_app
