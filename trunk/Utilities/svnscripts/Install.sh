#!/bin/bash

# Make sure all scripts are executable
chmod +x svnadds
chmod +x svnchanges
chmod +x svndiff
chmod +x svndiffer
chmod +x svnfind
chmod +x svnmissing
chmod +x svnunknowns

# Make symbolic links
ln -fs `pwd`/svnchanges $HOME/bin/svnchanges 
ln -fs `pwd`/svndiff $HOME/bin/svndiff 
ln -fs `pwd`/svnunknowns $HOME/bin/svnunknowns 
ln -fs `pwd`/svnadds $HOME/bin/svnadds 
ln -fs `pwd`/svnmissing $HOME/bin/svnmissing 
ln -fs `pwd`/svndiffer $HOME/bin/svndiffer 
