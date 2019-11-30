all:
	ccache g++ -c -std=c++11 -O0 -g -Wall -Wextra -Werror -pedantic Input.cpp -o Input.o
	ccache g++ -c -std=c++11 -O0 -g -Wall -Wextra -Werror -pedantic main.cpp -o main.o
	ccache g++ Input.o main.o
