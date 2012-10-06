INCLUDE=-I/opt/local/include

all:
	g++ -Wall -Wextra -Werror -I/opt/local/include -E main.cpp 2>&1 | tail -n 10 | head -n 5 | grep \(a

