#!/bin/bash
first=""
while read line; do [ "$first" == "" ] && first=$line || echo $(($line - $first)); done 
