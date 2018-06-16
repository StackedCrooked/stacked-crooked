echo >sampleText.txt

{ while read line; do echo "$line" >>sampleText.txt; sleep 0.1; done <rtt_over_time ; } &
python test.py
