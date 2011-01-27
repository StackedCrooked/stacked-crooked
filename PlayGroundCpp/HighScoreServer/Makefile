CPP_DEFINES=
INCLUDE_DIRS=-Iinclude -I/opt/local/include
LIBS= \
	-L/opt/local/lib \
	-lPocoFoundation \
	-lPocoNet \
	-lPocoXML \
	-lPocoUtil \
	-lPocoData \
	-lPocoSQLite


SRC = \
    src/ContentType.cpp \
    src/main.cpp \
    src/Renderer.cpp \
    src/RequestHandler.cpp \
    src/RequestHandlerFactory.cpp \
    src/RequestHandlerId.cpp \
    src/RequestMethod.cpp \
    src/ResourceId.cpp \
    src/Utils.cpp

all:
	g++ -o HighScoreServer $(INCLUDE_DIRS) $(CPP_DEFINES) $(LIBS) $(SRC)

clean:
	find . -name "*.o" | xargs rm

install:
	cp -f ./HighScoreServer /usr/bin/
	cp -f ./scripts/hsserv /etc/init.d/

start:
	/etc/init.d/hsserv start

stop:
	/etc/init.d/hsserv stop
