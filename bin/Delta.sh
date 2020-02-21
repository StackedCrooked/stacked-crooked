#!/bin/bash
awk '{
    for (i=1; i <= NF; i++) {
        a[NR][i] = $i
        if (NR > 1) {
            printf "%d", (a[NR][i] - a[NR - 1][i])
            if (i < NF) {
                printf " "
            }
        }
    }
    if (NR >= 2) {
        printf "\n"
    }
}'
