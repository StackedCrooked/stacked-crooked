all:
	g++ -o test main.cpp -lgtest

testsuite: all
	export GTEST_OUTPUT=xml
	./test

clean:
	rm -f ./test *.xml
