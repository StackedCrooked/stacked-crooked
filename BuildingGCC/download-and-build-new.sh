#!/bin/bash
set -e
set -x

VERSION="$(wget -O - https://ftp.gnu.org/gnu/gcc 2>/dev/null | grep -E -o 'gcc-[1-9]+\.[0-9]+\.[0-9]+' | sed -e 's,gcc-,,g' | sort -n | tail -n1)"
BASENAME="gcc-${VERSION}"
FILENAME="${BASENAME}.tar.xz"

FILE_URL="https://ftp.gnu.org/gnu/gcc/gcc-${VERSION}/${FILENAME}"

# Download the file
[ ! -f ${FILENAME} ] && {
    echo "Downloading ${FILE_URL}"
	wget "${FILE_URL}"
}

[ ! -d $BASENAME ] && { 
    echo "Extracting ${FILENAME}"
	tar xf "${FILENAME}"
}

# Enter source dir and download prerequisites
echo "Entering $BASENAME and downloading prerequisites"
(cd $BASENAME && ./contrib/download_prerequisites)

# Exit the source dir, create the build directory and enter it
echo "Building the source code"
mkdir build && cd build

echo "Configuring"
../$BASENAME/configure --disable-multilib --enable-languages=c,c++

echo "Building"
make -j$(nproc)

echo "Make install-strip!"
make install-strip

