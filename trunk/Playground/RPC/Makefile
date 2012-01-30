CPP_FILES=Asio.cpp RPCServer.cpp Stopwatch.cpp TCPServer.cpp TCPClient.cpp
O_FILES=Asio.o RPCServer.o Stopwatch.o TCPServer.o TCPClient.o
HEADERS= Asio.h Client.h RemoteCall.h RemoteObject.h RemoteObjects.h RPCServer.h Serialization.h Stopwatch.h TCPServer.h TCPClient.h
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBDIR=/usr/lib
LIBS=${LIBDIR}/libboost_system.a ${LIBDIR}/libboost_serialization.a -lpthread

all: server client

server: ${O_FILES} TestServer.o 
	g++ -ggdb -o server TestServer.o ${O_FILES} ${LIBS}

client: ${O_FILES} TestClient.o
	g++ -ggdb -o client TestClient.o ${O_FILES} ${LIBS}

TCPClient.o: TCPClient.cpp
	${COMPILE} TCPClient.cpp

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
