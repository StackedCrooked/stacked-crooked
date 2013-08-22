while read line ; do
    line_length="${#line}"
    offset="$(($line_length - ${1}))"
    echo $line | cut -c-${offset}
done
