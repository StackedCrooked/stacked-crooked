#!/bin/bash

echo "Timestamp Packets" >Stream.csv
echo "Timestamp Packets" >Trigger.csv


# Stream config
pps=1000 # pps
duration=10

# Network latency 
latency_ms=15 

# Initial timestamp (start of flow)
timestamp=1

for i in $(seq $duration); do
    head_start=$(($pps * $latency_ms / 1000))
    echo "$timestamp $(($pps + $head_start))" >> Stream.csv
    echo "$timestamp $pps" >> Trigger.csv
    timestamp=$(($timestamp + 1))
done

# Final snapshot
echo "$timestamp 0" >> Stream.csv
echo "$timestamp $head_start" >> Trigger.csv



{
    echo "Timestamp TxPackets RxPackets"
    q -H "select tx.Timestamp, tx.Packets, rx.Packets from Stream.csv tx join Trigger.csv rx on (tx.Timestamp = rx.Timestamp)"
} | column -t
