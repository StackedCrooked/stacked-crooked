#!/bin/sh
[ -d $HOME/bin ] || {
    echo "$HOME/bin not found. Exiting." 1>&2
    exit 1
}

for dir in "Common $(uname)" ; do
    for file in $(find $(dirname $0)/$dir -type f -name '*.sh') ; do
        echo ln -fs $file $HOME/bin/$(basename $file)
    done
done

