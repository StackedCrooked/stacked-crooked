sum=0
while read line; do sum="$(($line + $sum))"; done
echo "$sum"

