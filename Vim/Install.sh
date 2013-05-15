set -x
set -e
ln -fs `pwd`/_vimrc $HOME/.vimrc
ln -fs `pwd`/_gvimrc $HOME/.gvimrc
if [  `uname` == "Darwin" ]; then
    echo "Link ~/bin/gvim to MacVim"
    ln -fs `pwd`/MacVim $HOME/bin/gvim
fi

apt-get install git || port install git || exit 
[ ! -d ~/.vim/bundle/vundle ] || { echo "Vundle is already installed. Ok." ; exit ; }

mkdir -p ~/.vim/bundle
git clone https://github.com/gmarik/vundle ~/.vim/bundle/vundle
vim +BundleInstall +qall
