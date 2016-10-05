all:
	g++ main.cpp -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread 
	g++ bpf.cpp -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread -lpcap -o bpf_test
