all:
	g++ -std=c++11 -DNUM_ITERATIONS=10 -DARRAY_SIZE=10000000 -O2 -Wall -Wextra -Werror main.cpp && ./a.out; rm -f a.out
	g++ -std=c++11 -DNUM_ITERATIONS=1000 -DARRAY_SIZE=100000 -O2 -Wall -Wextra -Werror main.cpp && ./a.out; rm -f a.out
	g++ -std=c++11 -DNUM_ITERATIONS=100000 -DARRAY_SIZE=1000 -O2 -Wall -Wextra -Werror main.cpp && ./a.out; rm -f a.out
