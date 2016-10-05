all:
	g++ main.cpp -DBPF_FILTER=1 -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread  -lpcap -o ./benchmark_bpf
	g++ main.cpp -DBPF_FILTER=0 -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread  -lpcap -o ./benchmark_native
	#g++ bpf.cpp -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread -lpcap -o bpf_test
