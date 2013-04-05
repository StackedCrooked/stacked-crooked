
all:
	g++ -o test   -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/usr/local/lib -pthread main.cpp        -lboost_system -lboost_thread
	g++ -o server -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/usr/local/lib -pthread chat_server.cpp -lboost_system -lboost_thread
	g++ -o client -std=gnu++0x -Wall -Wextra -Werror -pedantic-errors -isystem /usr/local/include -L/usr/local/lib -pthread chat_client.cpp -lboost_system -lboost_thread
