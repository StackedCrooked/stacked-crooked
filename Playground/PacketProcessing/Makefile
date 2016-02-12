all:
	g++ main.cpp -std=c++11 -Wall -Wextra -Werror -O2 -funroll-loops -ftree-vectorize -march=native -pthread 
	echo >Results.txt
	./a.out | tee -a Results.txt
