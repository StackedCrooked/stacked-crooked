
all:
	g++ -o server -std=c++0x -g -Wall -Wextra -Werror -pedantic-errors -I/opt/local/include -L/opt/local/lib \
	main.cpp Networking.cpp \
	-pthread \
	/opt/local/lib/libboost_system-mt.a \
	/opt/local/lib/libboost_thread-mt.a \
	/opt/local/lib/libboost_chrono-mt.a

