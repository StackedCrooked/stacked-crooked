CPP_DEFINES = -ggdb3
INCLUDE_DIRS = -Iinclude -I/opt/local/include
LIBS = \
	-L/opt/local/lib \
	-lPocoFoundation \
	-lPocoUtil \
	-lPocoNet

SRC = main.cpp

all:
	g++ -o server -std=c++11 -Wall -Wextra -Werror -pedantic-errors $(INCLUDE_DIRS) $(CPP_DEFINES) $(LIBS) $(SRC)

clean:
	find . -type f -name "*.o" | xargs rm
