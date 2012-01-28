INCLUDE=-I/opt/local/include
LIB=-L/opt/local/lib
FILES=$(shell find . -name *.cpp | grep -v Test.cpp)

all: run-client run-server

run-client: ${FILES} Client/ClientTest.cpp Client/UDPClient.cpp
	g++ -Wall -Wextra -Werror -O0 -ggdb ${INCLUDE} -I$(shell pwd) ${LIB} -o run-client ${FILES} Client/UDPClient.cpp Client/ClientTest.cpp -lboost_system -lboost_serialization 

run-server: ${FILES} Server/ServerTest.cpp
	g++ -Wall -Wextra -Werror -O0 -ggdb ${INCLUDE} -I$(shell pwd) ${LIB} -o run-server ${FILES} Server/ServerTest.cpp -lboost_system -lboost_serialization 

