ln -fs `pwd`/_vimrc $HOME/.vimrc
ln -fs `pwd`/_gvimrc $HOME/.gvimrc
if [  `uname` == "Darwin" ]; then
    echo "Link ~/bin/gvim to MacVim"
    ln -fs `pwd`/MacVim $HOME/bin/gvim
else
    echo "No gvim on this platform. OK."
fi
