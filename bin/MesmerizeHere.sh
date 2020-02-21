#!/bin/bash
[ "$#" == "0" ] && {
    #MESMERIZE_AUTOLOAD="$(pwd)" open -n /Users/francis/programming/projects/Mesmerize/build/Mesmerize.app
    MESMERIZE_AUTOLOAD="$(pwd)" open -n /Users/francis/programming/projects/build-Mesmerize-Desktop_Qt_5_8_0_clang_64bit-Release/Mesmerize.app

    exit
}

for i in "$@" ; do
    (cd "$i" && echo "pwd=$(pwd)" && MESMERIZE_AUTOLOAD="$(pwd)" open -n /Applications/Mesmerize.app/Contents/MacOS/Mesmerize)
done

#A
