
all:
	g++ -o test -ggdb -I/opt/local/include -Iinclude main.cpp src/ScopedMuter.cpp

clean:
	find . -name "*.o" | xargs rm
	rm test
	rm -rf test.dSYM
