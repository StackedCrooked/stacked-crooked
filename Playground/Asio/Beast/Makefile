all:
	ccache /usr/bin/g++ -std=c++11 -O2 -L/usr/local/lib -isystem /usr/local/include async_client.cpp -o async_client -lboost_system
	ccache /usr/bin/g++ -DBOOST_COROUTINES_NO_DEPRECATION_WARNING -std=c++11 -O2 -L/usr/local/lib -isystem /usr/local/include coro_client.cpp -o coro_client -lboost_system -lboost_coroutine
