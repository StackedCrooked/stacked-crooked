ls *.cpp | grep -v boost | while read line ; do echo "--- $(cat $line) --- " ; export line ; echo "preprocessed size: $(/usr/bin/g++ -std=c++11 -c -E $line | wc -l) lines" ; { time /usr/bin/g++ -std=c++11 -c "$line" -O2 ; } 2>&1 | grep -v '^$' ; echo ; echo ; done 2>&1 | grep -v 'user\|sys'  
