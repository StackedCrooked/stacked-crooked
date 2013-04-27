#!/bin/bash
[ $# -eq 1 ] || { echo "Usage $0 FileName" 1>&2 && exit 1 ; }
SRC_FILE=$1
DEFAULT_CMD='g++-4.8 -std=c++11 -O2 -pthread main.cpp && ./a.out'
CMD=${COLIRU_CMD:-${DEFAULT_CMD}}
POST_FILE=$(mktemp -t coliru_post_file)

# Write the file
echo "${CMD}__COLIRU_SPLIT__$(cat ${SRC_FILE})" > ${POST_FILE}

OUTPUT_FILE=$(mktemp -t coliru_post_file)
wget -nv "http://coliru.stacked-crooked.com/compile2" --post-file=${POST_FILE} -O ${OUTPUT_FILE} 2>/dev/null
cat ${OUTPUT_FILE}

