all: tcp_client tcp_server

tcp_client:
	g++ -std=c++11 -O2 -Wall -Wextra TCPClient.cpp -o tcp_client -lboost_system

tcp_server:
	g++ -std=c++11 -O2 -Wall -Wextra TCPServer.cpp -o tcp_server -lboost_system

clean:
	rm tcp_client tcp_server

