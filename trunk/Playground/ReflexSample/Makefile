INCLUDE = -I/usr/local/include
SRC = main.cpp Person.cpp Person_MetaData.cpp
LIB = -L/usr/local/lib -lReflex
CFLAGS = -Wl,--rpath,/usr/local/lib -ggdb3

all: reflex
	g++ -o test $(CFLAGS) $(INCLUDE) $(LIB) $(SRC) 

reflex:
	genreflex Person.h -o Person_MetaData.cpp

clean:
	rm -f test
	rm -f *.o
	rm -f *.cxx
