#!/bin/bash
sort -n | awk '{
    for (i = 1; i <= NF; i++) {
        numbers[NF][NR] = $1
    }
    count++
} END {
    if (count % 2 != 0) {
        # Be careful to avoid indexing with a non-integer (count/2 is no integer)
        for (i = 1; i <= NF; i++) {
            printf "%d", numbers[i][(count+1)/2]
            if (i < NF) {
                printf " "
            }
        }
    } else {
        for (i = 1; i <= NF; i++) {
            a = numbers[i][count/2]
            b = numbers[i][count/2 + 1]
            printf "%d",(a + b)/2
            if (i < NF) {
                printf " "
            }
        }
    }
    printf "\n"
}'
