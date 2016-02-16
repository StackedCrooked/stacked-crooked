all:
	g++ main.cpp -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread 
	echo >Results.txt
	which perf && { perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,branch-misses,faults,migrations ./a.out | tee -a Results.txt ; } || { ./a.out | tee -a Results.txt ;}
	echo Done
