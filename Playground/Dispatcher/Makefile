CPP_DEFINES = -ggdb3
INCLUDE_DIRS = -Iinclude -I/opt/local/include
LIBS = \
    -L/opt/local/lib \
    -lPocoFoundation \
	-lPocoNet

SRC = main.cpp

all:
	g++ -o server $(INCLUDE_DIRS) $(CPP_DEFINES) $(LIBS) $(SRC)

clean:
	find . -type f -name "*.o" | xargs rm
