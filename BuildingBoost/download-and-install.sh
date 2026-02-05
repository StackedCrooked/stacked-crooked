#!/bin/bash
set -e
set -x
LATEST_VERSION=$(curl -s https://www.boost.org/releases/latest/ | grep -Pio 'Latest \(\d\.\d\d\.\d.*?\)' | grep -Po '\d\.\d\d\.\d')
echo "Latest version: ${LATEST_VERSION}"

DIRNAME="boost_$(echo $LATEST_VERSION  | perl -pe 's,\.,_,g')"
echo "Dirname: ${DIRNAME}"

TARNAME="${DIRNAME}.tar.gz"
echo "Tarfile name: ${TARNAME}"

echo wget "https://archives.boost.io/release/${LATEST_VERSION}/source/${TARNAME}"
wget "https://archives.boost.io/release/${LATEST_VERSION}/source/${TARNAME}"

echo "Untar ${TARNAME}"
tar xf "${TARNAME}"

echo "Enter directly ${DIRNAME}"
cd "${DIRNAME}"

# Boostrap ?
./bootstrap.sh

# Build + Install
./b2 cxxstd=20 --layout=system variant=release threading=multi link=static install
