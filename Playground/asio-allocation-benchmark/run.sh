#!/bin/bash
set -e
[ -f ./build/a.out ] || make
./build/a.out
