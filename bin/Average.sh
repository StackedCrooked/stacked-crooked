#!/bin/bash

sum=0
count=0

while read line; do
    sum=$(($line + $sum))
    count=$(($count + 1))
done


[ "$count" != "0" ] && {
    echo $(($sum / $count))
} || {
    echo 0
}
