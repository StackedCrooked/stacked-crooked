FILES=Client.cpp Server.cpp Stopwatch.cpp Networking.cpp
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBS=/opt/local/lib/libboost_system.a /opt/local/lib/libboost_serialization.a

all: server client

server: Server.o Stopwatch.o Networking.o
	g++ -o server Server.o Stopwatch.o Networking.o ${LIBS}

client: Client.o Stopwatch.o Networking.o
	g++ -o client Client.o Stopwatch.o Networking.o ${LIBS}

Client.o: Client.cpp
	${COMPILE} Client.cpp
 
Server.o: Server.cpp
	${COMPILE} Server.cpp

Stopwatch.o: Stopwatch.cpp
	${COMPILE} Stopwatch.cpp

Networking.o: Networking.cpp
	${COMPILE} Networking.cpp
