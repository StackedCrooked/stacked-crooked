#!/bin/bash
while read line ; do export BLOCKSIZE=1 ; echo "$(du -s "$line" | awk '{print $1}') $line" ; done
