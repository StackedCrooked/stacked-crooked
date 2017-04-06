num_columns="$(awk '{print NF}' "${1}" | sort -nu | tail -n 1)"
command="set terminal png size 1600,900; set grid; set output '$1.png'; set key outside; plot '$1' using 1:2:xtic(1) with linespoints"
gnuplot -e "$command" && xdg-open "${1}.png"
