
all:
	g++-4.7 -o server -std=gnu++0x -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/opt/local/lib -pthread TestServer.cpp -lboost_system -lboost_thread
	g++-4.7 -o client -std=gnu++0x -ggdb3 -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/opt/local/lib -pthread TestClient.cpp -lboost_system -lboost_thread
