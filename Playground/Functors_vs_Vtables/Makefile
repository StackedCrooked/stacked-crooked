all:
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 VTable.cpp
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 VTableImpl.cpp
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 VTableTest.cpp
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 Functor.cpp
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 FunctorImpl.cpp
	g++ -std=c++11 -c -Wall -Wextra -Werror -O2 FunctorTest.cpp
	g++ VTable*.o -o vtable_test
	g++ Functor*.o -o functor_test
