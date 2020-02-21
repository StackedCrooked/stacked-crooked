#!/bin/bash

awk '{
    sum += $1
    num_lines++
} END {
    print sum
}'
