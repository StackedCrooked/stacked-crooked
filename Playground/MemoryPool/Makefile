INCLUDE=-I/opt/local/include
CXXFLAGS=-Wall -Werror -O3
LFLAGS=-L/opt/local/lib -lPocoFoundation -lboost_thread-mt

all:
	g++ -o test $(INCLUDE) $(CXXFLAGS) $(LFLAGS) allocator.cpp pool.cpp main.cpp
