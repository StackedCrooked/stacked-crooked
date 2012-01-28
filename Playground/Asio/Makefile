INCLUDE=-I/opt/local/include
LIB=-L/opt/local/lib
FILES=$(shell find . -name *.cpp | grep -v main.cpp)

all: run-client run-server

run-client: ${FILES} Client/main.cpp
	g++ -Wall -Wextra -Werror -O0 -ggdb ${INCLUDE} -I$(shell pwd) ${LIB} -o run-client ${FILES} Client/main.cpp -lboost_system -lboost_serialization 

run-server: ${FILES} Server/main.cpp
	g++ -Wall -Wextra -Werror -O0 -ggdb ${INCLUDE} -I$(shell pwd) ${LIB} -o run-server ${FILES} Server/main.cpp -lboost_system -lboost_serialization 

