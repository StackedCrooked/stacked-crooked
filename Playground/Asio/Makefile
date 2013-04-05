
all:
	g++ -o test   -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread main.cpp        -lboost_system-mt -lboost_thread-mt
	g++ -o server -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread chat_server.cpp -lboost_system-mt -lboost_thread-mt
	g++ -o client -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /opt/local/include -L/opt/local/lib -pthread chat_client.cpp -lboost_system-mt -lboost_thread-mt
