FILES=$(shell find . -name *.cpp)

all:
	g++ -Wall -Wextra -Werror -O0 -ggdb -I$(shell pwd) -o test ${FILES} -lboost_serialization 
