#!/bin/bash
num_columns="$(awk '{print NF}' "${1}" | sort -nu | tail -n 1)"
command="set terminal png size 1600,900; set datafile missing \"-\"; set grid; set output '$1.png';  plot for [col=2:$num_columns] '$1' using 1:col:xtic(1) with linespoints title columnheader"
gnuplot -e "$command" && xdg-open "${1}.png"
