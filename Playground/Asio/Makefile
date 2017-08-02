
all:
	g++ -o server -std=c++11 -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread TestServer.cpp -lboost_system 
	g++ -o client -std=c++11 -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -pthread TestClient.cpp -lboost_system
