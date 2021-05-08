#!/bin/bash
set -e
set -x

LATEST_VERSION="$(wget -O - https://boostorg.jfrog.io/artifactory/main/release 2>/dev/null  | grep '[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*' | perl -pe 's,.*href="([\w\./]+)".*,\1,g' | sed -e 's,/$,,g' | tail -n1)"
echo "Latest version: ${LATEST_VERSION}"

DIRNAME="boost_$(echo $LATEST_VERSION  | perl -pe 's,\.,_,g')"
echo "Dirname: ${DIRNAME}"

TARNAME="${DIRNAME}.tar.gz"
echo "Tarfile name: ${TARNAME}"

echo wget https://boostorg.jfrog.io/artifactory/main/release/${LATEST_VERSION}/source/${TARNAME}
wget https://boostorg.jfrog.io/artifactory/main/release/${LATEST_VERSION}/source/${TARNAME}


echo "Untar ${TARNAME}"
tar xf "${TARNAME}"

echo "Enter directly ${DIRNAME}"
cd "${DIRNAME}"

# Boostrap ?
./bootstrap.sh

# Build + Install
./b2 --layout=system variant=release threading=multi link=static install


Build and install:

# Boostrap ?
./bootstrap.sh

# Build + Install
echo ./b2 --layout=system variant=release threading=multi link=static install
echo HAHA
