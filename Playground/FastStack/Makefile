
#pp:
	#while true ; do g++ -std=c++11 -E main.cpp > tmp && formatcode tmp && cat tmp | tail -n30 > tmp2 && clear && cat tmp2 && sleep 2 ; done

all:
	g++ -o test -std=c++11 -Wall -Wextra -Werror -pedantic-errors -ggdb3 -I/opt/local/include main.cpp && ./test
	g++ -o test -std=c++11 -Wall -Wextra -Werror -pedantic-errors -ggdb3 -I/opt/local/include GenericHeader.cpp && ./test
