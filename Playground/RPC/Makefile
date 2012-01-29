FILES=TestClient.cpp Networking.cpp RPCServer.cpp TestServer.cpp Stopwatch.cpp
HEADERS=RemoteCall.h Networking.h RemoteObject.h RemoteObjects.h RPCServer.h Serialization.h Stopwatch.h
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBS=/opt/local/lib/libboost_system.a /opt/local/lib/libboost_serialization.a 

all: server client

server: TestServer.o Stopwatch.o Networking.o RPCServer.o 
	g++ -ggdb -o server RPCServer.o TestServer.o Stopwatch.o Networking.o ${LIBS}

client: TestClient.o Stopwatch.o Networking.o 
	g++ -ggdb -o client TestClient.o Stopwatch.o Networking.o ${LIBS}

TestClient.o: TestClient.cpp ${HEADERS}
	${COMPILE} -DRPC_CLIENT TestClient.cpp

TestServer.o: TestServer.cpp ${HEADERS}
	${COMPILE} -DRPC_SERVER TestServer.cpp

Stopwatch.o: Stopwatch.cpp ${HEADERS}
	${COMPILE} Stopwatch.cpp

Networking.o: Networking.cpp ${HEADERS}
	${COMPILE} Networking.cpp

RPCServer.o: RPCServer.cpp ${HEADERS}
	${COMPILE} RPCServer.cpp
