#!/bin/bash
[ -d build ] || {
    mkdir build || (cd build && cmake ..)
}

(cd build && make -j7)

