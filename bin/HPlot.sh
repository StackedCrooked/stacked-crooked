#!/bin/bash
command="$(echo "set terminal png size 2000,1200
set boxwidth 0.8 relative
set style fill solid 1.0
set output '$1.png'
set xtic rotate by -45 scale 0
plot '$1' using 2:xtic(1) with boxes" | perl -pe 's,\n,;,g')"
gnuplot -e "$command" && xdg-open "${1}.png"
