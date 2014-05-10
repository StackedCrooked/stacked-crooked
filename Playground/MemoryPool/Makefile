INCLUDE=-I/opt/local/include -I$(shell pwd)
CXXFLAGS=-Wall -Werror -O3
LFLAGS=-L/opt/local/lib -lPocoFoundation

all:
	g++ -std=c++11 $(INCLUDE) $(CXXFLAGS) $(LFLAGS) *.cpp
