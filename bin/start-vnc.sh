#!/bin/bash


# Kill previous SSH reverse tunnels
ps -eopgid,comm,args | grep 'ssh -NR 5901:localhost:5901' | grep -v grep | awk '{print $1}' | while read line; do kill -9 -${line}; done


# Create a new SSH reverse tunnel 
while true; do ssh -NR 5901:localhost:5901 root@85.159.210.140; sleep 60; done & disown

# Start the VNC server
vncserver -kill :1
vncserver :1 -geometry 1920x1080


#vncserver :1 -geometry 2560x1440
