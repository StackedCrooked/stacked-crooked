#!/bin/bash
set -e
set -x

VERSION=9.3.0
NAME=gcc-${VERSION}


# Check if directory for the unpacked tarball is found
[ ! -d $NAME ] && {

    # We did not find the unpacked tarball diretory => check if the tarball (tar.xz) file is found!
    [ ! -f $NAME.tar.xz ] && {
        # The tar.xz file is not found => download it
        wget https://ftp.gnu.org/gnu/gcc/$NAME/$NAME.tar.xz
    }

    # We have the tarball (tar.xz) file => unpack it!
	tar xf $NAME.tar.xz 
}

# Enter the GCC source dir and download prerequisites
(cd $NAME && ./contrib/download_prerequisites)

# Exit the source dir, create the build directory and enter it
mkdir build && cd build

# NOTE: The GCC documentation recommends to NOT run make inside the GCC directory.

../$NAME/configure --prefix=/opt/$NAME --program-suffix=-${VERSION} --disable-multilib --enable-languages=c,c++

make -j$(nproc)
make install-strip

