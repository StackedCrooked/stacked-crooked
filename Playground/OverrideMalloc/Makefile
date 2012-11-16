all:
	g++ -std=c++0x -fPIC -c malloc_hook.cpp
	g++ -std=c++0x -shared -Wl,-soname,malloc_hook -ldl -o libmalloc_hook.so malloc_hook.o
	g++ -o test -std=c++0x main.cpp
