all:
	g++ -std=c++11 -O2 -fprofile-generate -Wall -Wextra -Werror main.cpp extern.cpp

final:
	g++ -std=c++11 -O2 -fprofile-use -Wall -Wextra -Werror main.cpp extern.cpp -o final.out

clean:
	rm -f *.o a.out
