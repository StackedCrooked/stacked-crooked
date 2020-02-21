#!/bin/bash
sort -n | awk '{
    numbers[NR] = $1
    count++
} END {
    if (count % 2 != 0) {
        # Be careful to avoid indexing with a non-integer (count/2 is no integer)
        print numbers[(count+1)/2]
    } else {
        a = numbers[count/2]
        b = numbers[count/2 + 1]
        print (a + b)/2
    }
}'
