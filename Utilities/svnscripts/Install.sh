#!/bin/bash
ln -fs `pwd`/svnchanges $HOME/bin/svnchanges 
ln -fs `pwd`/svndiff $HOME/bin/svndiff 
ln -fs `pwd`/svnunknowns $HOME/bin/svnunknowns 

# fmdiff is a Mac only tool
if [[ "`uname`" == 'Darwin' ]]; then
    ln -fs `pwd`/fmdiff $HOME/bin/fmdiff 
else
    echo "Skipping fmdiff (Mac only)."
fi

