num_columns="$(awk '{print NF}' "${1}" | sort -nu | tail -n 1)"
command="set terminal png size 1600,300; set grid; set output '$1.png'; set key outside; plot for [col=1:$num_columns] '$1' using 0:col with points title columnheader"
gnuplot -e "$command" && xdg-open "${1}.png"
