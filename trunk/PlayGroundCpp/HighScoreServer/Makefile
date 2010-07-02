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
	g++ -w -c -o ContentType.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/ContentType.cpp
	g++ -w -c -o Renderer.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/Renderer.cpp
	g++ -w -c -o RequestHandler.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestHandler.cpp
	g++ -w -c -o RequestHandlerId.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestHandlerId.cpp
	g++ -w -c -o RequestMethod.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestMethod.cpp
	g++ -w -c -o RequestHandlerFactory.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/RequestHandlerFactory.cpp
	g++ -w -c -o Utils.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/Utils.cpp
	g++ -w -c -o main.o $(INCLUDE_DIRS) $(CPP_DEFINES) src/main.cpp
	g++ -Wall *.o -o HighScoreServer $(LIBS)

clean:
	rm -f *.o

install:
	cp -f ./HighScoreServer /usr/bin/
	cp -f ./scripts/hsserv /etc/init.d/

