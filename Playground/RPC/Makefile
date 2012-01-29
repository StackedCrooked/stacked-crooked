CPP_FILES=Asio.cpp RPCServer.cpp Stopwatch.cpp TCPServer.cpp
O_FILES=Asio.o RPCServer.o Stopwatch.o TCPServer.o
HEADERS= Asio.h Client.h RemoteCall.h RemoteObject.h RemoteObjects.h RPCServer.h Serialization.h Stopwatch.h TCPServer.h
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBS=/opt/local/lib/libboost_system.a /opt/local/lib/libboost_serialization.a 

all: server client

server: ${O_FILES} TestServer.o
	g++ -ggdb -o server TestServer.o ${O_FILES} ${LIBS}

client: ${O_FILES} TestClient.o
	g++ -ggdb -o client TestClient.o ${O_FILES} ${LIBS}

TCPServer.o: TCPServer.cpp
	${COMPILE} TCPServer.cpp

TestClient.o: TestClient.cpp ${HEADERS}
	${COMPILE} -DRPC_CLIENT TestClient.cpp

TestServer.o: TestServer.cpp ${HEADERS}
	${COMPILE} -DRPC_SERVER TestServer.cpp

Stopwatch.o: Stopwatch.cpp ${HEADERS}
	${COMPILE} Stopwatch.cpp

Asio.o: Asio.cpp ${HEADERS}
	${COMPILE} Asio.cpp

RPCServer.o: RPCServer.cpp ${HEADERS}
	${COMPILE} RPCServer.cpp
