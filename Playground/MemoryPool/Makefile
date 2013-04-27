INCLUDE=-I/opt/local/include -I$(shell pwd)
CXXFLAGS=-Wall -Werror -O3
LFLAGS=-L/opt/local/lib -lPocoFoundation

all:
	g++ -o test $(INCLUDE) $(CXXFLAGS) $(LFLAGS) *.cpp
