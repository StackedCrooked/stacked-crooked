q -H --delimiter=','  "select tx.Time, tx.TxPackets, rx.RxPackets, (tx.TxPackets - rx.RxPackets) from Stream.csv tx join Trigger.csv rx on (tx.Time = rx.Time)"  
