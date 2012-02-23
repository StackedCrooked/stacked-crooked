#!/usr/bin/env sh
[ $# -ne 2 ] && echo "Usage: $(basename $0) ISOFile Target" && exit 1
mount -o loop $1 $2

