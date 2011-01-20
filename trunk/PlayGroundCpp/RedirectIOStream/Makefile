
all:
	g++ -o test -I/opt/local/include -Iinclude main.cpp src/ScopedMuter.cpp

debug:
	g++ -o test -ggdb -I/opt/local/include -Iinclude main.cpp src/ScopedMuter.cpp

clean:
	find . -name "*.o" | xargs rm
	rm test
	rm -rf test.dSYM
