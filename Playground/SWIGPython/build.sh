#!/bin/bash
rm -f *.o *.so
swig -c++ -python example.i
g++ -std=c++11 -O2 -fPIC -c example.cpp
g++ -std=c++11 -O2 -fPIC -c example_wrap.cxx -I/usr/include/python2.7
g++ -std=c++11 -shared example.o example_wrap.o -o _example.so
