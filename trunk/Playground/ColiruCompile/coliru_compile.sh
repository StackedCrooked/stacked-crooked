#!/bin/bash
[ $# -eq 1 ] || { echo "Usage $0 FileName" 1>&2 && exit 1 ; }

# Create a temp file that contains the http request
HTTP_REQUEST=$(mktemp -t coliru_http_request)

# Write the file
echo "${COLIRU_CMD:-'g++-4.8 -std=c++11 -O2 -pthread main.cpp && ./a.out'}__COLIRU_SPLIT__$(cat ${1})" > ${HTTP_REQUEST}

# Create a temp file that will contain the output
OUT_FILE=$(mktemp -t coliru_out_file)

# Create a temp file that will contain the error output
ERR_FILE=$(mktemp -t coliru_err_file)

{ wget -nv "http://coliru.stacked-crooked.com/compile2" --post-file=${HTTP_REQUEST} -O ${OUT_FILE} 2>${ERR_FILE} && cat ${OUT_FILE} ; } || cat ${ERR_FILE}

