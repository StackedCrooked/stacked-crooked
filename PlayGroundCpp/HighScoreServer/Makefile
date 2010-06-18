CPP_DEFINES=
INCLUDE_DIRS=-Iinclude
LIBS= \
    -lPocoFoundation \
	-lPocoNet \
	-lPocoXML \
	-lPocoUtil \
	-lPocoData \
	-lPocoSQLite

all:
	rm -f *.o
	g++ -w -c -o RequestHandler.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestHandler.cpp
	g++ -w -c -o RequestHandlerFactory.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestHandlerFactory.cpp
	g++ -w -c -o FileUtils.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/FileUtils.cpp
	g++ -w -c -o main.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/main.cpp
	g++ -Wall *.o -o HighScoreServer $(LIBS)


clean:
	rm -f *.o

install:
	mv -f ./HighScoreServer /usr/bin/

