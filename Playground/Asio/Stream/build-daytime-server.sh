#!/bin/bash
g++ -std=c++11 -O2 -g -Wall -Wextra -Werror DaytimeServer.cpp -o server -lboost_system
printf "To start the server run:\n\t./server\n"
