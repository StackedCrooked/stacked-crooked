OPTIONS = -DCOLIRU_SANDBOX_MEMORY_LIMIT=10 -std=c++0x -Wall -Wextra -Werror

all:
	g++ ${OPTIONS} -fPIC -c malloc_hook.cpp
	g++ ${OPTIONS} -shared -Wl,-soname,malloc_hook -ldl -o libmalloc_hook.so malloc_hook.o
	g++ ${OPTIONS} -o test main.cpp
