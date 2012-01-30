CORE_H_FILES= Core/Asio.h Core/RPCServer.h Core/Stopwatch.h Core/TCPClient.h Core/TCPServer.h
CORE_CPP_FILES= Core/Asio.cpp Core/RPCServer.cpp Core/Stopwatch.cpp Core/TCPClient.cpp Core/TCPServer.cpp
CORE_OBJ_FILES= Asio.o RPCServer.o Stopwatch.o TCPClient.o TCPServer.o
TEST_SERVER_CPP_FILES=TestServer.cpp
TEST_SERVER_OBJ_FILES=TestServer.o
TEST_CLIENT_CPP_FILES=TestClient.cpp
TEST_CLIENT_OBJ_FILES=TestClient.o
COMPILE=g++ -c -Wall -Wextra -Werror -O0 -ggdb -isystem /opt/local/include
LIBDIR=/usr/lib
LIBS=${LIBDIR}/libboost_system.a ${LIBDIR}/libboost_serialization.a -lpthread

all: server client

server: ${CORE_OBJ_FILES} ${TEST_SERVER_OBJ_FILES}
	g++ -ggdb -o server ${TEST_SERVER_OBJ_FILES} ${CORE_OBJ_FILES} ${LIBS}

client: ${CORE_OBJ_FILES} ${TEST_CLIENT_OBJ_FILES}
	g++ -ggdb -o client ${TEST_CLIENT_OBJ_FILES} ${CORE_OBJ_FILES} ${LIBS}

TestServer.o: TestServer/TestServer.cpp ${CORE_H_FILES}
	${COMPILE} -DRPC_SERVER TestServer/TestServer.cpp

TestClient.o: TestClient/TestClient.cpp ${CORE_H_FILES}
	${COMPILE} -DRPC_CLIENT TestClient/TestClient.cpp

TCPClient.o: Core/TCPClient.cpp ${CORE_H_FILES}
	${COMPILE} Core/TCPClient.cpp

TCPServer.o: Core/TCPServer.cpp ${CORE_H_FILES}
	${COMPILE} Core/TCPServer.cpp

Stopwatch.o: Core/Stopwatch.cpp ${CORE_H_FILES}
	${COMPILE} Core/Stopwatch.cpp

Asio.o: Core/Asio.cpp ${CORE_H_FILES}
	${COMPILE} Core/Asio.cpp

RPCServer.o: Core/RPCServer.cpp ${CORE_H_FILES}
	${COMPILE} Core/RPCServer.cpp
