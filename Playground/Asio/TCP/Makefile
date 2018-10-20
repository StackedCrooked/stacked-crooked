all: tcp_client tcp_server

tcp_client: TCPClient.cpp
	g++ -std=c++14 -DBOOST_ERROR_CODE_HEADER_ONLY -O2 -Wall -Wextra TCPClient.cpp

tcp_server: TCPServer.cpp
	g++ -std=c++14 -DBOOST_ERROR_CODE_HEADER_ONLY -O2 -Wall -Wextra TCPServer.cpp

clean:
	rm -f tcp_client tcp_server

