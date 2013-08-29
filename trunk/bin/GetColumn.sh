#!/bin/bash
[ "$#" == "1" ] || {
    echo "Usage $(basename $0) ColumnIndex" 1>&2
    echo "Example: ps -eouid,comm | grep 123 | $(basename $0) 2" 1>&2
    exit 1
}

eval "awk '{print \$$1}'"
