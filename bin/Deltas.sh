#!/bin/bash
prev=""
while read line; do [ "$prev" == "$line" ] && prev=$line || echo "$(($line - $prev))"; done
