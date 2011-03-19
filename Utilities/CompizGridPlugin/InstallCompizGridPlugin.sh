#!/bin/sh

if [ "$(whoami)" != "root" ]; then
  echo "Permission denied. Please run this script with root priviliges.";
  exit 1
fi

# Install dependencies
aptitude update
aptitude install git-core libtool compiz-dev compiz-fusion-bcop compizconfig-settings-manager

TEMPDIR=~/tmp/`date +%s`
COMPIZPLUGINDIR=$TEMPDIR/CompizGridPlugin

mkdir -p $COMPIZPLUGINDIR
cd $COMPIZPLUGINDIR
git clone git://anongit.compiz-fusion.org/compiz/plugins/grid
cd grid
git checkout 1281082ba678033e515a19419ca8ffe8641d744b
make
make install
ccsm 2>&1 > /dev/null &
