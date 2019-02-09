#!/bin/bash
num_columns="$(awk '{print NF}' "${1}" | sort -nu | tail -n 1)"
#"Time  Mbit/s  RTT  Retransmissions"
max_throughput="$(awk '{print $2}' "$1" | grep -v '[^0-9]' | sort -n | tail -n1)"
max_roundtriptime="$(awk '{print $3}' "$1" | grep -v '[^0-9]' | sort -n | tail -n1)"

echo "max_throughput=$max_throughput"
echo "max_roundtriptime=$max_roundtriptime"
command="set terminal png size 1200,400; set datafile missing \"-\"; set grid x y2;  set output '$1.png';
    set yrange [0:$((  ($max_throughput + 1000) / 1000 * 1000 ))];
    set ytics border;
    set y2range [0:$((  ($max_roundtriptime + 100) / 100 * 100))];
    set y2tics border;
    plot '$1' using 1:2 with linespoints title columnheader ,
         '$1' using 1:3 with linespoints title columnheader axes x1y2 "
gnuplot -e "$command" && xdg-open "${1}.png"
