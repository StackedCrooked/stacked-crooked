all:
	g++ -o test main.cpp -lgtest

testsuite: all
	rm *_detail.xml
	export GTEST_OUTPUT=xml
	./test
