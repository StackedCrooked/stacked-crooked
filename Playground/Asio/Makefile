
all:
	/Users/francis/bin/g++ -o http_client -std=c++11 -g -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread TCPClient.cpp -lboost_system 
	/Users/francis/bin/g++ -o echo_server -std=c++11 -g -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread EchoServer.cpp -lboost_system 
	/Users/francis/bin/g++ -o server -std=c++11 -g -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread TestServer.cpp -lboost_system 
	/Users/francis/bin/g++ -o client -std=c++11 -g -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread TestClient.cpp -lboost_system
