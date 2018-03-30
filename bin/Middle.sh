#!/bin/bash
file=$(mktemp)
trap "{ rm -f $file; }" EXIT

[ "$#" == 1 ] && {
    cp $1 $file
} || {
    while read line; do echo "$line" >> $file; done
}
    
num_lines="$(cat "$file" | wc -l | awk '{print $1}')"
[ "$num_lines" == "0" ] && {
    echo 0
    rm $file
    exit
}

middle_line="$(($num_lines / 2 + 1))"
head -n $middle_line $file | tail -n1
rm $file
