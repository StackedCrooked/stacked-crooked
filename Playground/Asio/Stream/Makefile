all: Stream.o main.o
	ccache g++ *.o -lboost_system -o stream

Stream.o: Stream.h Stream.cpp
	ccache g++ -c -std=c++11 -O2 -g -Wall -Wextra -Werror -pedantic-errors Stream.cpp

main.o: main.cpp Stream.h
	ccache g++ -c -std=c++11 -O2 -g -Wall -Wextra -Werror -pedantic-errors main.cpp



clean:
	rm -f *.o stream
