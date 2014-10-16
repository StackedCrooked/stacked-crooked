#!/bin/bash
./update-clang.sh && cd build && make "$@"
