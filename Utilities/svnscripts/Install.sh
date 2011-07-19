#!/bin/bash

# Make sure all scripts are executable
chmod 755 fmdiff
chmod 755 svnadds
chmod 755 svnchanges
chmod 755 svndiff
chmod 755 svnunknowns

# Make symbolic links
ln -fs `pwd`/svnchanges $HOME/bin/svnchanges 
ln -fs `pwd`/svndiff $HOME/bin/svndiff 
ln -fs `pwd`/svnunknowns $HOME/bin/svnunknowns 
ln -fs `pwd`/svnadds $HOME/bin/svnadds 
ln -fs `pwd`/svnmissing $HOME/bin/svnmissing 

# fmdiff is a Mac only tool
if [[ "`uname`" == 'Darwin' ]]; then
    ln -fs `pwd`/fmdiff $HOME/bin/fmdiff 
else
    echo "Skipping fmdiff (Mac only)."
fi

