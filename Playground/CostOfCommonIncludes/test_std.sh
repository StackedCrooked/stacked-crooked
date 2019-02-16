#!/bin/bash
export CXX="${CXX:-g++}"
echo "Using $(which "$CXX")" >&2
ls *.cpp | grep -v boost | while read line ; do export line ; echo "$(cat $line | cut -d'<' -f2 | cut -d'>' -f1) $("$CXX"  -std=c++11 -c -E $line | wc -l) $({ time "$CXX" -std=c++11 -c "$line" -O2 ; } 2>&1 | grep -v '^$' | sed -e 's,real,,g')" ;  done 2>&1 | grep -v 'user\|sys'  
