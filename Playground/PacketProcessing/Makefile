all:
	g++ main.cpp -DFILTER_VECTOR=1 -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread  -lpcap -o ./benchmark_vector
	g++ main.cpp -DFILTER_BPF=1 -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread  -lpcap -o ./benchmark_bpf
	g++ main.cpp -DFILTER_NATIVE=1 -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread  -lpcap -o ./benchmark_native
	#g++ bpf.cpp -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -march=native -pthread -lpcap -o bpf_test
