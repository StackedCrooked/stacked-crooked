#!/bin/bash
[ "$#" == "0" ] && {
    MESMERIZE_AUTOLOAD="$(pwd)" open -n /Users/francis/programming/projects/.builds/Mesmerize/release/Mesmerize.app
    exit
}

for i in "$@" ; do
    (cd "$i" && echo "pwd=$(pwd)" && MESMERIZE_AUTOLOAD="$(pwd)" open -n /Applications/Mesmerize.app/Contents/MacOS/Mesmerize)
done
