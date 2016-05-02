#!/bin/bash
rm -f *.o *.so
swig -c++ -python example.i
g++ -O2 -fPIC -c example.cpp
g++ -O2 -fPIC -c example_wrap.cxx -I/usr/include/python2.7
g++ -shared example.o example_wrap.o -o _example.so
