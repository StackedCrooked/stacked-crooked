all:
	g++ -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native main.cpp 
	g++ -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native Test.cpp
	g++ -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native Test2.cpp
	g++ -c -std=c++11 -Wall -Wextra -Werror -O2 -march=native Test3.cpp
	g++ main.o Test.o Test2.o Test3.o

clean:
	rm *.o
	rm a.out
