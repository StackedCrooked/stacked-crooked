#!/bin/bash
[ "$#" == "1" ] || { echo "Usage $(basename $0) URL" 1>&2 ; exit 1 ; }
lynx -dump -hiddenlinks=ignore "${1}" 2>&1 | perl -pe 's,.*(http.*),\1,g' | grep ^http 
