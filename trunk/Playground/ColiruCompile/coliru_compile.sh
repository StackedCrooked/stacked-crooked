#!/bin/bash

# Usage examples
# --------------
# Shell:
#   coliru_command.sh main.cpp
#
# Custom build command:
#   export COLIRU_CMD="g++ main.cpp"
#   coliru_command.sh main.cpp
# 
# Vim:
#   set make_prg=coliru_command.sh\ %
#   make

DEFAULT_SRC_FILE=main.cpp
[ $# -eq 1 ] || { echo "No input file was passed. Using ${DEFAULT_SRC_FILE}." ; }

SRC_FILE=${1:-${DEFAULT_SRC_FILE}}
[ -f ${SRC_FILE} ] || { echo "${SRC_FILE} not found. Exiting." 1>&1 ; } 

# Create a temp file that contains the http request
HTTP_REQUEST=$(mktemp -t coliru_http_request)

# Write the file
echo "${COLIRU_CMD:-g++-4.8 -std=c++11 -O2 -pthread main.cpp && ./a.out}__COLIRU_SPLIT__$(cat ${SRC_FILE})" > ${HTTP_REQUEST}

# Create a temp file that will contain the output
OUT_FILE=$(mktemp -t coliru_out_file)

# Create a temp file that will contain the error output
ERR_FILE=$(mktemp -t coliru_err_file)

{ wget -nv "http://coliru.stacked-crooked.com/compile2" --post-file=${HTTP_REQUEST} -O ${OUT_FILE} 2>${ERR_FILE} && cat ${OUT_FILE} ; } || cat ${ERR_FILE}
