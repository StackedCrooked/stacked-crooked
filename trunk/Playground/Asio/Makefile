
all:
	g++-mp-4.8 -o server -std=c++11 -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread TestServer.cpp -lboost_system-mt -lboost_thread-mt
	g++-mp-4.8 -o client -std=c++11 -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread TestClient.cpp -lboost_system-mt -lboost_thread-mt
