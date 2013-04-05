
all:
	g++-mp-4.8 -o test   -std=c++11 -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread main.cpp        -lboost_system-mt -lboost_thread-mt
	g++-mp-4.8 -o server -std=c++11 -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread chat_server.cpp -lboost_system-mt -lboost_thread-mt
	g++-mp-4.8 -o client -std=c++11 -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread chat_client.cpp -lboost_system-mt -lboost_thread-mt
