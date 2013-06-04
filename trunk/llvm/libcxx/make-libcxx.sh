#!/bin/bash

#
# Darwin build
#
if [ "$(uname)" == "Darwin" ] ; then
    cd libcxx/lib
    export TRIPLE=-apple-
    ./buildit
    exit
fi


#
# Linux build
#
mkdir -p build && cd build
export CC=clang
export CXX=clang++
cmake -G "Unix Makefiles" \
    -DLIBCXX_CXX_ABI=libsupc++ \
    -DLIBCXX_LIBSUPCXX_INCLUDE_PATHS="/usr/include/c++/4.8/;/usr/include/x86_64-linux-gnu/c++/4.8" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    ../libcxx && make -j3 && make install
