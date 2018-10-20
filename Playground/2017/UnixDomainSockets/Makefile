
all:
	ccache /usr/bin/g++ -std=c++11 -O2 -o stream_server stream_server.cpp -lboost_thread-mt -lboost_system-mt -pthread
	ccache /usr/bin/g++ -std=c++11 -O2 -o connect_pair connect_pair.cpp -lboost_system-mt -lboost_thread-mt -pthread
	ccache /usr/bin/g++ -std=c++11 -O2 -o stream_client stream_client.cpp -lboost_thread-mt -lboost_system-mt -pthread
