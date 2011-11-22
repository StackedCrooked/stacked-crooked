#!/bin/bash
SVN_SCRIPTS="svnadds svnchanges svnconflicts svndiff svndiffer svnfind svnmissing svnunknowns svnlog svnlogimpl"

for FILE in $SVN_SCRIPTS; do
    chmod +x $FILE
    ln -fs `pwd`/$FILE $HOME/bin/$FILE
done
