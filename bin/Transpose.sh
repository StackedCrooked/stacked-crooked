#!/bin/bash
awk '{ 
    for (i = 1; i <= NF; i++)  {
        a[NR,i] = $i
    }

    if (NF > num_columns) {
        num_columns = NF
    }
}
END {    
    for (j = 1; j <= num_columns; ++j) {
        for (i = 1; i <= NR; ++i) {
            if (i > 1) {
                printf(" ")
            }
            printf("%s", a[i,j]);
        }
        printf("\n")
    }
}'  "$1"
