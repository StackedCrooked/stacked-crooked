all:
	g++ -DSEMAPHORE=1 -std=c++11 -O2 -g -Wall -Wextra -pedantic -pthread main.cpp -o semaphore -Wno-deprecated-declarations
	g++ -DSEMAPHORE=0 -std=c++11 -O2 -g -Wall -Wextra -pedantic -pthread main.cpp -o condvar -Wno-deprecated-declarations

