FILES=Client.cpp Networking.cpp RPCServer.cpp Serialization.cpp Server.cpp Stopwatch.cpp Command.cpp
HEADERS=Command.h Commands.h Networking.h RemoteObject.h RemoteObjects.h RemotePtr.h RPCServer.h Serialization.h Stopwatch.h TupleSupport.h
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBS=/opt/local/lib/libboost_system.a /opt/local/lib/libboost_serialization.a 

all: server client

server: Serialization.o Server.o Stopwatch.o Networking.o RPCServer.o Command.o
	g++ -ggdb -o server RPCServer.o Server.o Stopwatch.o Networking.o Command.o ${LIBS}

client: Client.o Stopwatch.o Networking.o Command.o
	g++ -ggdb -o client Client.o Stopwatch.o Networking.o Command.o ${LIBS}

Client.o: Client.cpp ${HEADERS}
	${COMPILE} -DRPC_CLIENT Client.cpp

Command.o: Command.cpp ${HEADERS}
	${COMPILE} Command.cpp
 
Server.o: Server.cpp ${HEADERS}
	${COMPILE} -DRPC_SERVER Server.cpp

Stopwatch.o: Stopwatch.cpp ${HEADERS}
	${COMPILE} Stopwatch.cpp

Networking.o: Networking.cpp ${HEADERS}
	${COMPILE} Networking.cpp

RPCServer.o: RPCServer.cpp ${HEADERS}
	${COMPILE} RPCServer.cpp
