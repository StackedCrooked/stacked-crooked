ls | grep bench | while read line ; do echo "=== $line ==="; ./${line}; done
