#!/bin/bash
[ "$#" == "2" ] || {
    echo "Usage: $(basename $0) key=value file" 1>&2
    echo "File must contains key-value pairs using the format: 'key=value\n'. Each line must contain only one key=value statement." 1>&2
    echo "Example: $(basename $0) a=1 metadata.txt" 1>&2
    exit 1
}

assignment="$1"
file="$2"
key="$(echo $1 | cut -d'=' -f1)"
value="$(echo $1 | cut -d'=' -f2)"

# Check if a previous value was set.
old_value="$(grep $key $file)"
[ "$old_value" == "" ] &&  {
    echo "${key}=${value}" >> $file 
    echo "Added new key: ${key}=${value}"
    exit
}

# If key-value pair was found in file then
# we change the existing value to the new one.
cmd="perl -pe 's,${key}=.*,${key}=${value},' -i $file"
eval "$cmd"
echo "Changed ${old_value} to $(grep ${key} $file)"
