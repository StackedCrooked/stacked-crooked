#!/bin/bash
set -e
set -x

VERSION=9.3.0
NAME=gcc-${VERSION}



# Download the file
[ ! -f $NAME.tar.xz ] && {
	wget https://ftp.gnu.org/gnu/gcc/$NAME/$NAME.tar.xz
}

[ ! -d $NAME ] && { 
	tar xf $NAME.tar.xz 
}

# Enter source dir and download prerequisites
(cd $NAME && ./contrib/download_prerequisites)

# Exit the source dir, create the build directory and enter it
mkdir build && cd build

../$NAME/configure --prefix=/opt/$NAME --program-suffix=-${VERSION} --disable-multilib --enable-languages=c,c++

make -j$(nproc)
make install-strip

