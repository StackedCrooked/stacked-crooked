#!/bin/bash
while true ; do
    LOG="$(dirname $0)/history.log"
    echo "$(date) Start updating" >> $LOG
    [ $UID -ne 0 ] && echo "Must be run as root." && echo "`date` Insufficient priviliges." >> $LOG && exit 1
    port selfupdate && port upgrade outdated
    echo "`date` Finished updating" >> $LOG
    sleep $((25 * 3600))
done
