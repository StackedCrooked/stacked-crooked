#!/bin/bash
set -x

if [ $(whoami) != "root" ] ; then
	echo "Installation requires root permissions. Exiting." 1>&2
	exit 1
fi

apt-get install -y subversion vim aptitude software-properties-common python-software-properties
add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update
apt-get upgrade -y
BOOST=$(aptitude search boost | grep -E 'libboost[0-9]\.[0-9]+-all-dev[^:]' | awk '{print $2}' | sort -n -r | head -n1)
apt-get install -y g++-4.7 ccache ${BOOST} libpoco-dev
mkdir -p $HOME/bin
echo 'ccache g++-4.7 $*' >  $HOME/bin/g++ && chmod 755 $HOME/bin/g++
echo 'ccache gcc-4.7 $*' >  $HOME/bin/gcc && chmod 755 $HOME/bin/gcc
cd $HOME/bin && ln -fs `pwd`/g++ c++ && ln -fs `pwd`/gcc cc && cd -
cd Vim && ./Install.sh && cd -
cd Utilities/CodeFormatter && ./Install.sh && cd -
cd Utilities/svnscripts && ./Install.sh && cd -

echo 'export LC_ALL="en_US.UTF-8"' >> ~/.bashrc
echo 'LC_ALL=C' >> /etc/environment
