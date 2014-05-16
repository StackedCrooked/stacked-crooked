#!/bin/bash
while read line ; do echo "$(cat "$line") $line" ; done
