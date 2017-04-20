all:
	g++-5 -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native main.cpp 
	g++-5 -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native Test.cpp
	g++-5 -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native Test2.cpp
	g++-5 main.o Test.o Test2.o

clean:
	rm *.o
	rm a.out
