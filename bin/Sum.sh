#!/bin/bash
[ "$#" == 1 ] && {
    echo "$1"
    export sum=0
    cat "$1" | while read line; do sum="$(($line + $sum))"; done
    echo "$sum"
    exit 
}
    
sum=0
while read line; do sum="$(($line + $sum))"; done
echo "$sum"

