#!/bin/sh
[ -d $HOME/bin ] || {
    echo "$HOME/bin not found. Exiting." 1>&2
    exit 1
}

DIR="$(dirname $0)"
for dir in "$DIR/Common $DIR/$(uname)" ; do 
    for file in $(find $dir -type f) ; do
        ln -s $file $HOME/bin
    done
done

