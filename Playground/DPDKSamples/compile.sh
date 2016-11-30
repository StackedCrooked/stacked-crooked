#!/bin/bash
find . -maxdepth 1 -type d | grep -v '^\.$\|svn' | while read line ; do (set -x; cd "$line" && ./compile.sh) ; done
