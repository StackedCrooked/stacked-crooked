#!/bin/bash
[ "$#" == "1" ] || {
    echo "Usage: $(basename $0) file" 1>&2 
    exit 1
}

[ "$(uname)" == "Darwin" ] || {
    echo "$(basename $0) only works on Darwin." 1>&2
    exit 1
}

file="$1"

[ "$(echo "$file"| grep '\.app$')" ] && {
    file="$(find $file -name $(basename $file | sed -e 's,.app,,'))"
}


otool -L "$file" | tail -n +2 | perl -pe 's,\s*(\S+).*,\1,g' | grep -v '^@'
