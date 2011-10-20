make && valgrind --tool=memcheck --leak-check=full --num-callers=50 --show-reachable=yes --suppressions=./suppressions.supp ./test  2>valgrind.out && gvim valgrind.out 
