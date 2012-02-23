#!/usr/bin/env sh
[ $UID -ne 0 ] && echo "Must be run as root." && exit 1
port selfupdate && port upgrade outdated
