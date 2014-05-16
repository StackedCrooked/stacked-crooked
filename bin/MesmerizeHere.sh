#!/bin/bash
[ "$#" == "0" ] && {
    MESMERIZE_AUTOLOAD="$(pwd)" open -n /Applications/Mesmerize.app
    exit
}

for i in "$@" ; do
    (cd "$i" && echo "pwd=$(pwd)" && MESMERIZE_AUTOLOAD="$(pwd)" open -n /Applications/Mesmerize.app)
done
