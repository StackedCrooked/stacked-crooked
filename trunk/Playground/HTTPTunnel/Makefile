OPTIONS=-std=c++11 -ggdb3 -Wall -Wextra -Werror -pedantic-errors -I/opt/local/include -I/usr/include -L/opt/local/lib


SRC = Networking.cpp main.cpp
SRC_TARGETS = $(SRC:.cpp=)
SRC_OBJECTS = $(SRC:.cpp=.o)

%: %.cpp
	g++ ${OPTIONS} -c -o $*.o $*.cpp

all: ${SRC_TARGETS}
	g++ -o server ${OPTIONS} ${SRC_OBJECTS} -pthread -lboost_system-mt -lboost_thread-mt -lboost_chrono-mt

clean:
	rm -f *.o server

