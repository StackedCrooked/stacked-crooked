#!/bin/bash
[[ "$#" == "1" ]] || {
    echo "Usage: $(basename $0) num" 1>&2
    exit 1
}

while read line ; do
    line_length="${#line}"
    offset="$(($line_length - ${1}))"
    echo $line | cut -c-${offset}
done
