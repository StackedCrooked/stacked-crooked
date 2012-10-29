
for i in `seq 0 3 `; do
    COMMAND="g++ -o test -std=c++0x -O$i -Wall -Werror -Wextra -pedantic-errors main.cpp"
    echo $COMMAND
    $COMMAND && ./test
done
