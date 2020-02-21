#!/bin/bash
sort -n | awk '{
    for (i = 1; i <= NF; i++) {
        numbers[i][NR] = $i
    }
    num_columns = NF
    num_lines++
} END {
    if (num_lines % 2 != 0) {
        # Be careful to avoid indexing with a non-integer (num_lines/2 is no integer)
        for (i = 1; i <= num_columns; i++) {
            printf "%1$s", numbers[i][(num_lines+1)/2]
            if (i < num_columns) {
                printf " "
            }
        }
    } else {
        for (i = 1; i <= num_columns; i++) {
            a = numbers[i][num_lines/2]
            b = numbers[i][num_lines/2 + 1]
            avg = (a + b)/2
            printf "%1s",avg
            if (i < num_columns) {
                printf " "
            }
        }
    }
    printf "\n"
}'
