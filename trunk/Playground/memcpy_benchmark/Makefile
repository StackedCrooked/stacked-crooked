all:
	g++-mp-4.8 -std=c++11 -O3 -march=native -Wall -pedantic -pthread main.cpp && ./a.out
	g++-mp-4.8 -std=c++11 -O3 -march=native -Wall -pedantic -pthread threaded.cpp -o t && ./t 1 && ./t 2 && ./t 3 && ./t 4 && ./t 6 && ./t 8
