all: a.o b.o main.o
	g++ -o test *.o && ./test

clean:
	rm -f *.o test
	
a.o: a.cpp
	g++ -c -std=c++11 -Wall -Wextra -Werror -pedantic-errors a.cpp

b.o: b.cpp
	g++ -c -std=c++11 -Wall -Wextra -Werror -pedantic-errors b.cpp

main.o: main.cpp
	g++ -c -std=c++11 -Wall -Wextra -Werror -pedantic-errors main.cpp
