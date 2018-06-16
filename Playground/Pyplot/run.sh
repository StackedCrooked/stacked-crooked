echo >sampleText.txt

while read line; do echo "$line" >>sampleText.txt; done <rtt_over_time
python test.py
