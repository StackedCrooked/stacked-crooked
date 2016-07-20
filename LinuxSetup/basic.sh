#!/bin/bash
[ -d ~/stacked-crooked ] && {
	echo "Basic setup seems to be already done."
	exit 
}

sudo apt-get -y install subversion vim
svn co https://github.com/StackedCrooked/stacked-crooked.git/trunk ~/stacked-crooked
cp ~/stacked-crooked/Profile/_profile ~/.profile
cp  ~/stacked-crooked/Vim/_vimrc_minimal ~/.vimrc
