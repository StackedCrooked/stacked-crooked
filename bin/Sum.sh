#!/bin/bash

awk '{
    columns = NF
    for (i=1; i <= columns; i++) {
        sum[i] += $i
    }
}
END {
    for (i = 1; i <= columns; i++) {
        printf "%d", sum[i]

        # Print space after each column, except the last one
        if (i < columns) {
            printf " "
        }
    }
    printf "\n"
}'
