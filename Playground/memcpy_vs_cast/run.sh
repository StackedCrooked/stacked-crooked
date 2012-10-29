echo "Optimization level -O0" && g++ -o test -O0 -Wall -Werror -Wextra -pedantic-errors main.cpp && ./test
echo "Optimization level -O1" && g++ -o test -O1 -Wall -Werror -Wextra -pedantic-errors main.cpp && ./test
echo "Optimization level -O2" && g++ -o test -O2 -Wall -Werror -Wextra -pedantic-errors main.cpp && ./test
echo "Optimization level -O3" && g++ -o test -O3 -Wall -Werror -Wextra -pedantic-errors main.cpp && ./test
