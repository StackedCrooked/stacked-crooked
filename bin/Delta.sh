#!/bin/bash
i=0
while read line; do prev=$line; break; done
while read line; do echo $(($line - $prev)); prev=$line; done
