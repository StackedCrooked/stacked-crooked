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
	g++ -w -c -o HTMLElements.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/HTMLElements.cpp
	g++ -w -c -o HighScoreRequestHandler.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/HighScoreRequestHandler.cpp
	g++ -w -c -o HighScoreRequestHandlerFactory.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/HighScoreRequestHandlerFactory.cpp
	g++ -w -c -o HighScoreServer.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/HighScoreServer.cpp
	g++ -w -c -o main.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/main.cpp
	g++ -Wall *.o -o HighScoreServer $(LIBS)


clean:
	rm -f *.o


