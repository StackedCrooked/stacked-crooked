all:
	g++ -o test -std=c++0x malloc_hook.cpp main.cpp
	g++ -shared -W1,-soname,malloc_hook -o libmalloc_hook.so main.cpp
