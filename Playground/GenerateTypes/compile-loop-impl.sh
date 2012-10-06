g++ -Wall -Wextra -Werror -I/opt/local/include -E main.cpp | tail -n 50 | grep -v '#' | grep -v main
