all:
	g++ main.cpp -DPREFETCH=0 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread  && ./a.out
	g++ main.cpp -DPREFETCH=1 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread  && ./a.out
	g++ main.cpp -DPREFETCH=2 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread  && ./a.out
	g++ main.cpp -DPREFETCH=4 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread  && ./a.out
	g++ main.cpp -DPREFETCH=8 -std=c++11 -Wall -Wextra -Werror -O2 -march=native -pthread  && ./a.out
