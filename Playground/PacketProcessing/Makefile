all:
	g++-6 main.cpp -DPREFETCH=8 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread 
	echo >Results.txt
	./a.out | tee -a Results.txt
