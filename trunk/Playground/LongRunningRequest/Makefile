
all:
	g++ -o server -std=c++0x -g -Wall -Wextra -Werror -pedantic-errors -I/usr/include -L/usr/lib \
	main.cpp Networking.cpp \
	-pthread \
	-lboost_system-mt \
	-lboost_thread-mt \
	-lboost_chrono-mt

