all: echo_client echo_server

clean:
	rm echo_client echo_server

echo_client:
	g++ -std=c++11 -O2 -Wall -pedantic -pthread echo_client.cpp -o echo_client -lboost_system

echo_server:
	g++ -std=c++11 -O2 -Wall -pedantic -pthread echo_server.cpp -o echo_server -lboost_system
