#!/bin/bash
set -e
[ "$#" == "2" ] || {
    echo "Usage: $(basename $0) key_name file_name" 1>&2
    echo "File must contains key-value pairs using the format: 'key=value\n'. Each line must contain only one key=value statement."
    echo "Example: $(basename $0) upvotes metadata.txt"
    exit 1
}

key_name="$1"
file_name="$2"
grep $key_name $file_name >/dev/null 2>&1 || {
    echo "${key_name}=1" >> $file_name 
    echo "Added new key: ${key_name}=1" 1>&2
    exit
}
cmd="perl -pe 's,${key_name}=(\d+),\$n=\$1+1; \"${key_name}=\$n\",e'  -i $file_name"
eval "$cmd"
