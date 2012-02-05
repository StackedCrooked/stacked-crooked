COMPILE_FLAGS=-Wall -Wextra -Werror -O0 -ggdb
LINK_FLAGS=-L/opt/local/lib -lboost_serialization -lboost_system
FILES=RPC.cpp Networking.cpp main.cpp

all:
	g++ -o server -DRPC_SERVER=1 ${COMPILE_FLAGS} ${LINK_FLAGS} ${FILES} Stopwatch.cpp
	g++ -o client -DRPC_CLIENT=1 ${COMPILE_FLAGS} ${LINK_FLAGS} ${FILES}
