INCLUDE=-I/opt/local/include
LIB=-L/opt/local/lib
FILES=$(shell find . -name *.cpp)

all:
	g++ -std=c++0x -Wall -Wextra -Werror -O0 -ggdb ${INCLUDE} -I$(shell pwd) ${LIB} -o test ${FILES} -lboost_serialization 
