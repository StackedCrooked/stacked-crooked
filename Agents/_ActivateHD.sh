#!/bin/bash
ps aux | grep ActivateHD.sh | grep -v grep
ALREADY_RUNNING=$(ps aux | grep ActivateHD.sh | grep -v grep | wc -l | awk '{print $1}')
echo "ALREADY_RUNNING: ${ALREADY_RUNNING}"

[ ${ALREADY_RUNNING} -ge 1 ] || { say "Already running" 1>&2 && exit 1; }

say "Not yet running."

while true ; do 
	say 1
	echo "`date` activation" >> /Volumes/Lacie/Activate.txt
	say 2
	echo "`date` activation" >> /Volumes/WesternDigital/Activate.txt
	say 3
	sleep 10
	say 4
done
