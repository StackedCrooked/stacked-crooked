#!/bin/bash
while true; do ssh -fNR 5901:localhost:5901 root@85.159.210.140; sleep 60; done & disown
vncserver -kill :1
vncserver :1 -geometry 2560x1440
