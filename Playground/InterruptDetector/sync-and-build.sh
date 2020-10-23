#!/bin/bash
set -e
export remote_host="byteblower-dev-5100-2.lab.byteblower.excentis.com"
export remote_path="/root/tmp/InterruptChecker/"


scp -C main.cpp Makefile root@$remote_host:$remote_path


ssh root@$remote_host "
    set -e
    set -x
    cd $remote_path &&
    make &&
    ./a.out 2 SCHED_RR
"
