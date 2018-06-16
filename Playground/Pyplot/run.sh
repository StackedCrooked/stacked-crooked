echo >sampleText.txt

python test.py &

while read line; do echo "$line" >>sampleText.txt; sleep 0.1; done <rtt_over_time
