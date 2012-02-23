#!/usr/bin/env bash
[ $# -ne 1 ] && echo "Usage: $(basename $0) DirectoryName" && exit 1
isoimage -o myiso.iso $1
