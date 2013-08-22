#!/bin/bash
[ "$(uname)" == "Darwin" ] || {
    echo "$(basename $0) only works on Darwin." 1>&2
    exit 1
}

otool -L $1 | tail -n +2 | perl -pe 's,\s*(\S+).*,\1,g' | grep -v '^@'
