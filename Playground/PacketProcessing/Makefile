all:
	g++ main.cpp -DPREFETCH=1 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread 
	echo >Results.txt
	./a.out | tee -a Results.txt
