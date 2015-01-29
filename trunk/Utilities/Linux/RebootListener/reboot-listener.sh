#!/bin/bash

pipe=/home/francis/.reboot_requests


run() {
  rm -f $pipe
  mkfifo $pipe
  chmod a+w $pipe
  read line < $pipe
  /sbin/reboot
}



{ while true ; do run ; done ; } & disown
