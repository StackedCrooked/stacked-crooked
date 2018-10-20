#!/bin/bash
rm -rf Build
mkdir Build
cd Build
cmake ..
make -j10
