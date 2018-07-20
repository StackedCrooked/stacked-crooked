#!/bin/bash
[ -d Build ] || {
    mkdir Build
    (cd Build && cmake ..)
}

(cd Build && make clean && make -j8)
