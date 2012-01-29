FILES=Client.cpp Networking.cpp RPCServer.cpp Serialization.cpp Server.cpp Stopwatch.cpp
HEADERS=Command.h Commands.h Networking.h RemoteObject.h RemoteObjects.h RemotePtr.h RPCServer.h Serialization.h Stopwatch.h TupleSupport.h
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBS=/opt/local/lib/libboost_system.a /opt/local/lib/libboost_serialization.a 

all: server client

server: Serialization.o Server.o Stopwatch.o Networking.o RPCServer.o
	g++ -ggdb -o server RPCServer.o Server.o Stopwatch.o Networking.o ${LIBS}

client: Client.o Stopwatch.o Networking.o
	g++ -ggdb -o client Client.o Stopwatch.o Networking.o ${LIBS}

Client.o: Client.cpp ${HEADERS}
	${COMPILE} Client.cpp
 
Server.o: Server.cpp ${HEADERS}
	${COMPILE} -DTARGET_IS_RPC_SERVER Server.cpp

Stopwatch.o: Stopwatch.cpp ${HEADERS}
	${COMPILE} Stopwatch.cpp

Networking.o: Networking.cpp ${HEADERS}
	${COMPILE} Networking.cpp
