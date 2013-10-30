#!/bin/bash
set -x
set -e

[ -f  $HOME/.gvimrc ] && unlink $HOME/.gvimrc
[ -f  $HOME/.vimrc ] && unlink $HOME/.vimrc
rm -rf $HOME/.viminfo
rm -rf $HOME/.vim


check_installed() {
    type $1 >/dev/null 2>&1 || {
        echo "Please install $1 first." 1>&2 
        exit 1
    }
}

check_installed git
check_installed curl


LinkRCFiles_Darwin() {
  ln -fs `pwd`/_vimrc $HOME/.vimrc
  ln -fs `pwd`/_gvimrc $HOME/.gvimrc
  mkdir -p $HOME/bin
  echo "mvim $@" > $HOME/bin/gvim
  chmod +x $HOME/bin/gvim
}

LinkRCFiles_Linux() {
  ln -fs `pwd`/_vimrc $HOME/.vimrc
  ln -fs `pwd`/_gvimrc $HOME/.gvimrc
  echo "/usr/bin/gvim $@" > $HOME/bin/gvim
  chmod +x $HOME/bin/gvim
}

LinkRCFiles_$(uname)


# Install pathogen
mkdir -p ~/.vim/autoload ~/.vim/bundle; \
curl -Sso ~/.vim/autoload/pathogen.vim \
    https://raw.github.com/tpope/vim-pathogen/master/autoload/pathogen.vim

# Install Vundle
[ -d ~/.vim/bundle/vundle ] || { 
  git clone https://github.com/gmarik/vundle.git ~/.vim/bundle/vundle
  vim +BundleInstall +qall
}
