#!/bin/bash
[ "$#" == "1" ] || { echo "Usage $(basename $0) URL" 1>&2 ; exit 1 ; }
URL=${1}
LOG_DIR="logging_$(date '+%s')" && mkdir -p ${LOG_DIR}

LOG_FILE_01="${LOG_DIR}/01_RawData"
LOG_FILE_02="${LOG_DIR}_02_ExtractedURLs"


clog () {
    $1 $2 > $3
    cat $3
}

clog cat ${URL} ${LOG_FILE_01}
clog cat ${LOG_FILE_01} | ExtractURLs.sh > ${LOG_FILE_02}
cat ${LOG_FILE_02}

