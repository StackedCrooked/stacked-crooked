#!/bin/bash
buffer=""
count=0
newline="$(printf "\n")"
while read line; do
    buffer="$(printf "${line}\n${buffer}")"
    count=$(($count+1))
done

#printf "BUFFER:\n$buffer\b"
printf "$buffer" | sort -n | head -n $(($count/2)) | tail -n1
