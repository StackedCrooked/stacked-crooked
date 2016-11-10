all:
	rm *.o
	g++ -c -std=c++11 -O2 -march=native -Wall -Wextra -Werror Networking.cpp
	g++ -c -std=c++11 -O2 -march=native -Wall -Wextra -Werror Utils.cpp
	g++ simple.cpp Networking.o Utils.o -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -g -march=native -pthread  -lpcap  -o simple
	g++ with-hash.cpp -std=c++11 -Wall -Wextra -pedantic -Werror -O2 -g -march=native -pthread  -lpcap -o with-hash
