#!/bin/bash
set -e
set -x

VERSION=9.3.0



# Download the file
wget https://ftp.gnu.org/gnu/gcc/gcc-${VERSION}/gcc-${VERSION}.tar.xz

# Enter source dir and download prerequisites
(cd gcc-${VERSION} && ./contrib/download_prerequisites)

# Exit the source dir, create the build directory and enter it
mkdir build && cd build

../gcc-${VERSION}/configure --prefix=/opt/gcc-${VERSION} --disable-multilib --enable-languages=c,c++

make -j$(nproc)
make install-strip
