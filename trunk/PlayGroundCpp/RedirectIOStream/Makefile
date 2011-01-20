
all:
	g++ -o test -Wall -Werror -I/opt/local/include main.cpp ScopedMuter.cpp ScopedRedirect.cpp

clean:
	find . -name "*.o" | xargs rm
	rm test
	rm -rf test.dSYM
