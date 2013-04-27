
all:
	g++ -o server -std=c++11 -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/opt/local/lib -pthread TestServer.cpp -lboost_system -lboost_thread
	g++ -o client -std=c++11 -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/opt/local/lib -pthread TestClient.cpp -lboost_system -lboost_thread
