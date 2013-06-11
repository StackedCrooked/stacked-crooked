#!/bin/bash
set -x

if [ $(whoami) != "root" ] ; then
	echo "Installation requires root permissions. Exiting." 1>&2
	exit 1
fi

apt-get install -y subversion vim aptitude software-properties-common python-software-properties
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get update -y
apt-get upgrade -y
cd Vim && ./Install.sh && cd -
cd Utilities/svnscripts && ./Install.sh && cd -

echo 'export LC_ALL="en_US.UTF-8"' >> ~/.bashrc
echo 'LC_ALL=C' >> /etc/environment
