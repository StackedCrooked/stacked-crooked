#!/bin/bash
awk '{ 
    for (i = 1; i <= NF; i++)  {
        a[NR,i] = $i
    }

    if (NF > num_fields) {
        num_fields = NF
    }
}
END {    
    for (j = 1; j <= num_fields; j++) {
        str = a[1,j]
        for(i = 2; i <= NR; i++){
            str = str" "a[i,j];
        }
        print str
    }
}'  "$1"
