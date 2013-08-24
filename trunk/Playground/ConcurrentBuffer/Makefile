all:
	rm -f a.out 
	/opt/local/bin/g++-mp-4.8 \
		-std=c++11 \
		-O0 \
		-ggdb3 \
		-Wall \
		-Wextra \
		-pedantic-errors \
		-pthread \
		-isystem \
		/opt/local/include \
		/opt/local/lib/libtbb.dylib \
		main.cpp
