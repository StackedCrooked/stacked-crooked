INCLUDE=-I/opt/local/include

all: cpp build

build:
	g++ -o test $(INCLUDE) main.cpp

cpp:
	g++ $(INCLUDE) -E main.cpp > main_preprocessed.cpp

