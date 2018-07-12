
all:
	ccache /usr/bin/g++ -std=c++11 -O2 -g -Wall -Wextra -Werror -pedantic -isystem /usr/local/include -lboost_system TestUDPClient.cpp Networking.cpp -pthread -o udp_client
