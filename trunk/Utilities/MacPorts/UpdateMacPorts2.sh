#!/usr/bin/env sh
LOG=`dirname $0`/history.log
echo "`date` Start updating" >> $LOG
[ $UID -ne 0 ] && echo "Must be run as root." && exit 1
port selfupdate && port upgrade outdated
echo "`date` Finished updating" >> $LOG
