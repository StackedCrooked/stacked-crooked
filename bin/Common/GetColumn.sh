#!/bin/sh
[ "$#" == "1" ] || {
    echo "Usage $(basename $0) ColumnIndex" 1>&2
    echo "Example: ps -eouid,comm | grep 123 | $(basename $0) 2" 1>&2
    exit 1
}

COLUMN="$1"
COMMAND="awk '{print \$$1}'"
eval "$COMMAND"
