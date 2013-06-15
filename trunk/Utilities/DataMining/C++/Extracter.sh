#!/bin/bash
FILE_SH=$(basename "$0")
EXT="${FILE_SH##*.}"
FILE="${FILE_SH%.*}"
echo "FILE_SH: ${FILE_SH}"
echo "EXT: ${EXT}"
echo "FILE: ${FILE}"

[ -f "${FILE}" ] || { echo "Building ${FILE}" &&  g++ -std=c++11 -O3 ${FILE}.cpp -o ${FILE} && echo "Please restart." && exit 1 ; }  || { echo "Build failed." 1>&2 && exit 1 ; }
./${FILE}
